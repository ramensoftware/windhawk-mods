import * as child_process from 'child_process';
import { createHash } from 'crypto';
import * as fs from 'fs';
import * as path from 'path';
import ModSourceUtils from './modSourceUtils.js';

// Selects the submissions that catalog_external.json lists: each open pull
// request that was handed over to a human reviewer, at the commit it was handed
// over at, until it's closed. The previous deploy's catalog is the only state.
// A full scan reads every open pull request. An incremental scan reads only the
// pull requests updated since the previous deploy, and carries the other
// listings over as they are.

const REPO = 'ramensoftware/windhawk-mods';

const WAITING_FOR_REVIEWER = 'waiting-for-reviewer';

// The windhawk-reviewer app, which posts the AI reviews.
const AI_REVIEWER_LOGIN = 'windhawk-reviewer[bot]';

// The commit an AI review covers, as pr_flow.cjs reads it.
const AI_REVIEW_MARKER_PATTERN = /<!--\s*ai-review\s+sha=([0-9a-f]{40})\s*-->/g;

// A pull request with an AI review that has this section is never listed.
const AI_REVIEW_HARMFUL_SECTION = '<summary>Potentially misleading or harmful';

// Covers the time between the previous deploy's scan and its commit.
const INCREMENTAL_SCAN_MARGIN_MS = 60 * 60 * 1000;

// A pull request listed at one of its commits.
type Listing = {
    prNumber: number;
    id: string;
    commit: string;
    published: number;
    updated: number;
};

type PullRequestSummary = {
    number: number;
    open: boolean;
    atReviewer: boolean;
};

type Revision = {
    // With CRLF line endings, which is what the app stores and hashes.
    source: string;
    sha256: string;
};

export type ExternalModListing = {
    id: string;
    source: string;
    sourceUrl: string;
    sha256: string;
    pullRequestUrl: string;
    published: number;
    updated: number;
};

function getPullRequestUrl(prNumber: number) {
    return `https://github.com/${REPO}/pull/${prNumber}`;
}

// Through the base repository, which also serves the commits of a fork.
function getSourceUrl(id: string, commit: string) {
    return `https://raw.githubusercontent.com/${REPO}/${commit}/mods/${id}.wh.cpp`;
}

function readPreviousListings(lastDeployPath: string): Map<number, Listing> | null {
    const catalogPath = path.join(lastDeployPath, 'catalog_external.json');
    if (!fs.existsSync(catalogPath)) {
        return null;
    }

    const catalog = JSON.parse(fs.readFileSync(catalogPath, 'utf8'));
    const listings = new Map<number, Listing>();
    for (const [id, mod] of Object.entries<any>(catalog.mods)) {
        const prNumber = parseInt(mod.pullRequest?.match(/\/pull\/(\d+)$/)?.[1], 10);
        const commit = mod.source?.url?.match(/\/([0-9a-f]{40})\/mods\//)?.[1];
        if (mod.pullRequest !== getPullRequestUrl(prNumber) || mod.source.url !== getSourceUrl(id, commit)) {
            throw new Error(`Unexpected external catalog entry ${id}`);
        }

        listings.set(prNumber, {
            prNumber,
            id,
            commit,
            published: mod.details.published,
            updated: mod.details.updated,
        });
    }

    return listings;
}

function getLastDeployTime(lastDeployPath: string) {
    const result = child_process.spawnSync('git', ['-C', lastDeployPath, 'log', '-1', '--format=%ct'], {
        encoding: 'utf8',
    });
    if (result.status !== 0) {
        throw new Error(`Reading the last deploy time failed with status ${result.status} and stderr ${result.stderr}`);
    }

    return parseInt(result.stdout, 10) * 1000;
}

async function githubGet(endpoint: string): Promise<any> {
    const url = `https://api.github.com/repos/${REPO}/${endpoint}`;
    const response = await fetch(url, {
        headers: {
            'Accept': 'application/vnd.github+json',
            'Authorization': `Bearer ${process.env.GITHUB_TOKEN}`,
            'User-Agent': 'nodejs',
        },
    });
    if (!response.ok) {
        throw new Error(`Request to ${url} failed with status ${response.status}: ${await response.text()}`);
    }

    return response.json();
}

async function githubGetAll(endpoint: string): Promise<any[]> {
    const items: any[] = [];
    const separator = endpoint.includes('?') ? '&' : '?';
    for (let page = 1; ; page++) {
        const pageItems = await githubGet(`${endpoint}${separator}per_page=100&page=${page}`);
        items.push(...pageItems);
        if (pageItems.length < 100) {
            return items;
        }
    }
}

function summarizePullRequest(item: any): PullRequestSummary {
    return {
        number: item.number,
        open: item.state === 'open',
        atReviewer: item.labels.some((label: { name: string }) => label.name === WAITING_FOR_REVIEWER),
    };
}

// A pull request opened while paging shifts the later ones by one, so a pull
// request can be listed twice, and the map keeps one of them.
async function listOpenPullRequests() {
    const pullRequests = await githubGetAll('pulls?state=open&base=main');
    return new Map(pullRequests.map(pr => [pr.number, summarizePullRequest(pr)]));
}

// Labeling, comments, pushes and closing all update a pull request's
// updated_at. Closed pull requests are included, so that their listings are
// dropped.
async function listPullRequestsUpdatedSince(time: number) {
    const issues = await githubGetAll(`issues?state=all&since=${new Date(time).toISOString()}`);
    return new Map(issues
        .filter(issue => issue.pull_request)
        .map(issue => [issue.number, summarizePullRequest(issue)]));
}

// Only the AI reviewer's own comments count, since anyone can quote a review.
async function readAiReviews(prNumber: number) {
    const comments = await githubGetAll(`issues/${prNumber}/comments`);

    let lastReviewedCommit: string | null = null;
    let flagged = false;

    // Comments come back oldest first, so the last marker found is the newest one.
    for (const comment of comments) {
        if (comment.user?.login !== AI_REVIEWER_LOGIN) {
            continue;
        }

        const body: string = comment.body ?? '';
        const markers = [...body.matchAll(AI_REVIEW_MARKER_PATTERN)];
        if (markers.length === 0) {
            continue;
        }

        lastReviewedCommit = markers[markers.length - 1][1];
        flagged ||= body.includes(AI_REVIEW_HARMFUL_SECTION);
    }

    return { lastReviewedCommit, flagged };
}

// The byte order mark is kept, as the app keeps it.
async function fetchRevision(id: string, commit: string): Promise<Revision | null> {
    const url = getSourceUrl(id, commit);
    const response = await fetch(url, { headers: { 'User-Agent': 'nodejs' } });
    if (response.status === 404) {
        console.warn(`${url} wasn't found`);
        return null;
    }

    if (!response.ok) {
        throw new Error(`Request to ${url} failed with status ${response.status}`);
    }

    const bytes = await response.arrayBuffer();

    let text: string;
    try {
        text = new TextDecoder('utf-8', { fatal: true, ignoreBOM: true }).decode(bytes);
    } catch {
        console.warn(`${url} isn't valid UTF-8`);
        return null;
    }

    const source = text.replace(/\r\n|\r|\n/g, '\r\n');
    const sha256 = createHash('sha256').update(source, 'utf8').digest('hex');
    return { source, sha256 };
}

async function readHandOver(
    pull: any,
    regularModIds: Set<string>,
    modSourceUtils: ModSourceUtils,
): Promise<{ id: string; revision: Revision } | null> {
    const prNumber: number = pull.number;

    const files = await githubGetAll(`pulls/${prNumber}/files`);
    const match = files.length === 1 ? files[0].filename.match(/^mods\/([0-9a-z-]+)\.wh\.cpp$/) : null;
    if (!match) {
        console.warn(`#${prNumber}: Expected a single mod file`);
        return null;
    }

    // The app ignores a submission for an id that the catalog has.
    const id = match[1];
    if (regularModIds.has(id)) {
        return null;
    }

    const revision = await fetchRevision(id, pull.head.sha);
    if (!revision) {
        return null;
    }

    let metadata;
    try {
        metadata = modSourceUtils.extractMetadata(revision.source, 'en-US');
    } catch (e) {
        console.warn(`#${prNumber}: ${e}`);
        return null;
    }

    // The app takes new revisions only from the same @github, so it must name
    // the pull request's author.
    const expectedGithub = `https://github.com/${pull.user.login}`;
    if (metadata.id !== id || metadata.github?.toLowerCase() !== expectedGithub.toLowerCase()) {
        console.warn(`#${prNumber}: Expected @id ${id} and @github ${expectedGithub}`);
        return null;
    }

    return { id, revision };
}

export async function getExternalModListings(regularModIds: Set<string>): Promise<ExternalModListing[]> {
    const lastDeployPath = process.env.WINDHAWK_MODS_LAST_DEPLOY_PATH;
    if (!lastDeployPath) {
        throw new Error('WINDHAWK_MODS_LAST_DEPLOY_PATH is not set');
    }

    const modSourceUtils = new ModSourceUtils('mods');
    const now = Date.now();
    const previous = readPreviousListings(lastDeployPath);

    // Pushes are frequent, and get an incremental scan. Any other run gets a
    // full scan, which also repairs whatever an incremental scan missed.
    const incremental = process.env.GITHUB_EVENT_NAME === 'push' && previous !== null;
    const pullRequests = incremental
        ? await listPullRequestsUpdatedSince(getLastDeployTime(lastDeployPath) - INCREMENTAL_SCAN_MARGIN_MS)
        : await listOpenPullRequests();

    const listings: Listing[] = [];
    const revisions = new Map<string, Revision>();

    if (incremental) {
        for (const listing of previous.values()) {
            if (!pullRequests.has(listing.prNumber)) {
                listings.push(listing);
            }
        }
    }

    for (const pr of pullRequests.values()) {
        const prev = previous?.get(pr.number);
        if (!pr.open || (!pr.atReviewer && !prev)) {
            continue;
        }

        const { lastReviewedCommit, flagged } = await readAiReviews(pr.number);
        if (flagged) {
            console.log(`#${pr.number}: Not listed, an AI review found potentially misleading or harmful areas`);
            continue;
        }

        // Back with its author, a pull request stays listed at the commit it
        // was handed over at.
        let listing = prev;

        if (pr.atReviewer) {
            const pull = await githubGet(`pulls/${pr.number}`);
            const commit: string = pull.head.sha;

            // A push takes the pull request out of the queue, but the label can
            // lag behind it.
            if (pull.base.ref === 'main' && commit === lastReviewedCommit) {
                const handOver = await readHandOver(pull, regularModIds, modSourceUtils);
                if (!handOver) {
                    continue;
                }

                revisions.set(commit, handOver.revision);
                listing = {
                    prNumber: pr.number,
                    id: handOver.id,
                    commit,
                    published: prev?.published ?? now,
                    updated: prev?.commit === commit ? prev.updated : now,
                };
            }
        }

        if (listing) {
            listings.push(listing);
        }
    }

    // One listing per id. The pull request that had the id in the previous
    // deploy keeps it, otherwise the lowest number gets it.
    const wasListed = (listing: Listing) => previous?.get(listing.prNumber)?.id === listing.id;
    listings.sort((a, b) => Number(wasListed(b)) - Number(wasListed(a)) || a.prNumber - b.prNumber);

    const result: ExternalModListing[] = [];
    const listedIds = new Set<string>();

    for (const listing of listings) {
        // Merged, or another mod with the same id joined the catalog.
        if (regularModIds.has(listing.id)) {
            continue;
        }

        if (listedIds.has(listing.id)) {
            console.warn(`#${listing.prNumber}: ${listing.id} is listed for another pull request`);
            continue;
        }

        const revision = revisions.get(listing.commit) ?? await fetchRevision(listing.id, listing.commit);
        if (!revision) {
            continue;
        }

        listedIds.add(listing.id);
        result.push({
            id: listing.id,
            source: revision.source,
            sourceUrl: getSourceUrl(listing.id, listing.commit),
            sha256: revision.sha256,
            pullRequestUrl: getPullRequestUrl(listing.prNumber),
            published: listing.published,
            updated: listing.updated,
        });
    }

    console.log(
        `Listing ${result.length} external mods, after ${incremental ? 'an incremental' : 'a full'} scan` +
        ` of ${pullRequests.size} pull requests`);

    return result;
}
