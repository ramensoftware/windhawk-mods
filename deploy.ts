import * as child_process from 'child_process';
import * as fs from 'fs';
import * as https from 'https';
import * as path from 'path';
import ModSourceUtils from './modSourceUtils.js';
import { Feed } from 'feed';
import { OutgoingHttpHeaders } from 'http';
import showdown from 'showdown';

type ModAuthorData = {
    github: string;
    author: string;
    homepages: string[],
    twitter?: string;
};

type CommitMeta = {
    timestamp: number;
    message: string;
};

type FileChange = {
    changeType: string;
    filePath: string;
};

class GitCache {
    commitMeta: Map<string, CommitMeta>;
    commitOrder: string[];
    modCommits: Map<string, { commit: string; changeType: string }[]>;
    commitFiles: Map<string, FileChange[]>;
    blobs: Map<string, string>;

    constructor(data: {
        commitMeta: Map<string, CommitMeta>;
        commitOrder: string[];
        modCommits: Map<string, { commit: string; changeType: string }[]>;
        commitFiles: Map<string, FileChange[]>;
        blobs: Map<string, string>;
    }) {
        this.commitMeta = data.commitMeta;
        this.commitOrder = data.commitOrder;
        this.modCommits = data.modCommits;
        this.commitFiles = data.commitFiles;
        this.blobs = data.blobs;
    }

    getCommitMeta(commit: string): CommitMeta {
        const meta = this.commitMeta.get(commit);
        if (!meta) {
            throw new Error(`No metadata for commit ${commit}`);
        }
        return meta;
    }

    getModCommits(modId: string): { commit: string; changeType: string }[] {
        const entries = this.modCommits.get(modId);
        if (!entries || entries.length === 0) {
            throw new Error(`No commit history for mod ${modId}`);
        }
        return entries;
    }

    getBlob(ref: string): string {
        const content = this.blobs.get(ref);
        if (content === undefined) {
            throw new Error(`Blob not found in cache: ${ref}`);
        }
        return content;
    }
}

// Inspired by https://gist.github.com/ktheory/df3440b01d4b9d3197180d5254d7fb65
async function fetchJson(url: string, headers?: OutgoingHttpHeaders) {
    return new Promise<any>((resolve, reject) => {
        const req = https.request(url,
            { headers: { 'User-Agent': 'nodejs', ...headers } },
            (res) => {
                let body = '';
                res.on('data', (chunk) => (body += chunk.toString()));
                res.on('error', reject);
                res.on('end', () => {
                    if (res.statusCode && res.statusCode >= 200 && res.statusCode <= 299) {
                        resolve(JSON.parse(body));
                    } else {
                        reject('Request failed. status: ' + res.statusCode + ', body: ' + body);
                    }
                });
            });
        req.on('error', reject);
        req.end();
    });
}

// https://stackoverflow.com/a/53593328
function JSONstringifyOrder(obj: any, space: number) {
    const allKeys = new Set<string>();
    JSON.stringify(obj, (key, value) => {
        allKeys.add(key);
        return value;
    });
    return JSON.stringify(obj, Array.from(allKeys).sort(), space);
}

function gitExec(args: string[]) {
    const result = child_process.spawnSync('git', args, { encoding: 'utf8' });
    if (result.status !== 0) {
        throw new Error('git ' + args.join(' ') + ' failed with status ' + result.status + ' and stderr ' + result.stderr);
    }

    if (result.stderr) {
        console.warn('git ' + args.join(' ') + ' produced stderr ' + result.stderr);
    }

    return result.stdout;
}

function batchReadBlobs(refs: string[]): Map<string, string> {
    if (refs.length === 0) {
        return new Map();
    }

    const result = child_process.spawnSync('git', ['cat-file', '--batch'], {
        input: refs.join('\n') + '\n',
        maxBuffer: 512 * 1024 * 1024,
    });

    if (result.status !== 0) {
        throw new Error('git cat-file --batch failed with status ' + result.status +
            ' and stderr ' + (result.stderr?.toString() ?? ''));
    }

    const output = result.stdout;
    const map = new Map<string, string>();
    let offset = 0;

    for (let i = 0; i < refs.length; i++) {
        const headerEnd = output.indexOf(0x0A, offset);
        if (headerEnd === -1) {
            throw new Error(`Unexpected end of cat-file output at ref ${refs[i]}`);
        }
        const header = output.subarray(offset, headerEnd).toString('utf8');

        if (header.endsWith('missing')) {
            throw new Error(`git cat-file: ${refs[i]} → ${header}`);
        }

        const size = parseInt(header.split(' ')[2], 10);
        const contentStart = headerEnd + 1;
        const content = output.subarray(contentStart, contentStart + size).toString('utf8');
        map.set(refs[i], content);
        offset = contentStart + size + 1;
    }

    return map;
}

function buildGitCache(): GitCache {
    console.log('Building git cache...');

    // Bulk 1: All commit metadata (hash, timestamp, message).
    // Uses double-null as record separator, single-null as field separator.
    console.log('  Loading commit metadata...');
    const metaOutput = gitExec(['log', '--format=%H%x00%ct%x00%B%x00%x00']);
    const commitMeta = new Map<string, CommitMeta>();
    const commitOrder: string[] = [];

    for (const record of metaOutput.split('\0\0')) {
        const trimmed = record.replace(/^\n/, '');
        if (!trimmed) {
            continue;
        }
        const firstNull = trimmed.indexOf('\0');
        const secondNull = trimmed.indexOf('\0', firstNull + 1);
        const hash = trimmed.slice(0, firstNull);
        const timestamp = parseInt(trimmed.slice(firstNull + 1, secondNull), 10);
        const message = trimmed.slice(secondNull + 1);
        commitMeta.set(hash, { timestamp, message });
        commitOrder.push(hash);
    }
    console.log(`  ${commitMeta.size} commits`);

    // Bulk 2: All commits with their changed files (name-status).
    // Null byte before hash makes commit boundaries unambiguous.
    console.log('  Loading commit file changes...');
    const fileOutput = gitExec(['log', '--format=%x00%H', '--name-status', '--no-renames']);
    const modCommits = new Map<string, { commit: string; changeType: string }[]>();
    const commitFiles = new Map<string, FileChange[]>();

    const chunks = fileOutput.split('\0');
    // First chunk is always empty (before the leading null byte).
    if (chunks[0].trim()) {
        throw new Error('Unexpected content before first null byte in git log output');
    }

    for (let ci = 1; ci < chunks.length; ci++) {
        const chunk = chunks[ci];
        if (!chunk.trim()) {
            // Trailing empty chunk after last null byte.
            if (ci !== chunks.length - 1) {
                throw new Error('Unexpected empty chunk in git log output');
            }
            break;
        }
        const lines = chunk.split('\n');
        const hash = lines[0].trim();
        if (!/^[0-9a-f]{40}$/.test(hash)) {
            throw new Error(`Expected a commit hash, got '${hash}'`);
        }

        const files: FileChange[] = [];
        for (let i = 1; i < lines.length; i++) {
            const line = lines[i];
            if (!line) {
                // Blank lines separate the hash from file entries.
                continue;
            }
            const statusMatch = line.match(/^([A-Z]\d*)\t(.+)$/);
            if (!statusMatch) {
                throw new Error(`Unexpected name-status line in commit ${hash}: '${line}'`);
            }
            const changeType = statusMatch[1][0];
            const filePath = statusMatch[2].split('\t').pop()!; // last part handles renames

            files.push({ changeType, filePath });

            const match = filePath.match(/^mods\/(.+)\.wh\.cpp$/);
            if (match) {
                const modId = match[1];
                if (!modCommits.has(modId)) {
                    modCommits.set(modId, []);
                }
                modCommits.get(modId)!.push({ commit: hash, changeType });
            }
        }
        commitFiles.set(hash, files);
    }
    console.log(`  ${modCommits.size} mods, ${commitFiles.size} commits with file data`);

    // Bulk 3: Pre-fetch all mod blobs via cat-file --batch.
    console.log('  Pre-fetching mod blobs...');
    const blobRefs: string[] = [];
    for (const [modId, entries] of modCommits) {
        for (const entry of entries) {
            if (entry.changeType !== 'D') {
                blobRefs.push(`${entry.commit}:mods/${modId}.wh.cpp`);
            }
        }
    }
    const blobs = batchReadBlobs(blobRefs);
    console.log(`  ${blobs.size} blobs loaded`);

    return new GitCache({ commitMeta, commitOrder, modCommits, commitFiles, blobs });
}

function getModCreatedTime(modId: string, cache: GitCache) {
    const entries = cache.getModCommits(modId);
    const oldest = entries[entries.length - 1];
    if (oldest.changeType !== 'A') {
        throw new Error(`Expected first commit for mod ${modId} to be an add, got '${oldest.changeType}'`);
    }
    return cache.getCommitMeta(oldest.commit).timestamp * 1000;
}

function getModModifiedTime(modId: string, cache: GitCache) {
    const entries = cache.getModCommits(modId);
    return cache.getCommitMeta(entries[0].commit).timestamp * 1000;
}

function findCachedMod(modId: string, version: string, arch: string) {
    const lastDeployPath = process.env.WINDHAWK_MODS_LAST_DEPLOY_PATH;
    if (!lastDeployPath) {
        throw new Error('WINDHAWK_MODS_LAST_DEPLOY_PATH is not set');
    }

    const modFile = path.join(lastDeployPath, 'mods', modId, `${version}_${arch}.dll`);
    return fs.existsSync(modFile) ? modFile : null;
}

function compileMod(modFilePath: string, output32FilePath: string, output64FilePath: string, outputArm64FilePath: string) {
    const windhawkPath = process.env.WINDHAWK_PATH;
    if (!windhawkPath) {
        throw new Error('WINDHAWK_PATH is not set');
    }

    const result = child_process.spawnSync('py', [
        'scripts/compile_mod.py',
        '-w',
        windhawkPath,
        '-f',
        modFilePath,
        '-o32',
        output32FilePath,
        '-o64',
        output64FilePath,
        '-oarm64',
        outputArm64FilePath,
    ], { encoding: 'utf8', stdio: 'inherit' });
    if (result.status !== 0) {
        throw new Error('Compiling ' + modFilePath + ' failed with status ' + result.status);
    }
}

function validateAndUpdateAuthorData(
    modId: string,
    commit: string,
    metadata: {
        github: string;
        author: string;
        homepage?: string;
        twitter?: string;
    },
    modAuthorData: Record<string, ModAuthorData>
) {
    const authorKey = metadata.github.toLowerCase();
    if (!modAuthorData[authorKey]) {
        modAuthorData[authorKey] = {
            github: metadata.github,
            author: metadata.author,
            homepages: [],
        };
    }

    const entry = modAuthorData[authorKey];

    if (metadata.homepage !== undefined) {
        if (!entry.homepages.includes(metadata.homepage)) {
            entry.homepages.push(metadata.homepage);
        }
    }

    if (entry.twitter === undefined && metadata.twitter !== undefined) {
        entry.twitter = metadata.twitter;
    }

    const inconsistencies: string[] = [];

    if (metadata.github !== entry.github) {
        inconsistencies.push(`github: expected '${entry.github}', got '${metadata.github}'`);
    }

    if (metadata.author !== entry.author) {
        // Allow specific known author name variations
        const allowedPairs = [
            ['CatmanFan / Mr._Lechkar', 'CatmanFan'],
            ['Anixx', 'anixx'],
            ['Isabella Lulamoon (kawapure)', 'kawapure'],
        ];

        const matchedPair = allowedPairs.find(pair =>
            pair.includes(metadata.author) && pair.includes(entry.author)
        );

        if (matchedPair) {
            // Normalize to the first item in the pair
            entry.author = matchedPair[0];
        } else {
            inconsistencies.push(`author: expected '${entry.author}', got '${metadata.author}'`);
        }
    }

    if (metadata.twitter !== undefined && metadata.twitter !== entry.twitter) {
        inconsistencies.push(`twitter: expected '${entry.twitter}', got '${metadata.twitter}'`);
    }

    if (inconsistencies.length > 0) {
        throw new Error(
            `Mod ${modId} has inconsistent author data in commit ${commit}:\n` +
            inconsistencies.map(msg => `  - ${msg}`).join('\n')
        );
    }
}

function handleCompiledFiles(
    modId: string,
    modDir: string,
    modVersionFilePath: string,
    metadata: { version: string; architecture?: string[] }
) {
    const modVersionCompiled32FilePath = path.join(modDir, `${metadata.version}_32.dll`);
    const modVersionCompiled64FilePath = path.join(modDir, `${metadata.version}_64.dll`);
    const modVersionCompiledArm64FilePath = path.join(modDir, `${metadata.version}_arm64.dll`);
    const cachedMod32Path = findCachedMod(modId, metadata.version, '32');
    const cachedMod64Path = findCachedMod(modId, metadata.version, '64');
    const cachedModArm64Path = findCachedMod(modId, metadata.version, 'arm64');

    if (cachedMod32Path || cachedMod64Path || cachedModArm64Path) {
        const modHas32 = metadata.architecture?.includes('x86') ?? true;
        const modHas64 =
            (metadata.architecture?.includes('x86-64') ?? true) ||
            (metadata.architecture?.includes('amd64') ?? true);
        const modHasArm64 =
            (metadata.architecture?.includes('x86-64') ?? true) ||
            (metadata.architecture?.includes('arm64') ?? true);
        if (modHas32 != !!cachedMod32Path || modHas64 != !!cachedMod64Path || modHasArm64 != !!cachedModArm64Path) {
            throw new Error(`Mod ${modId} architecture mismatch`);
        }

        if (cachedMod32Path) {
            fs.copyFileSync(cachedMod32Path, modVersionCompiled32FilePath);
        }

        if (cachedMod64Path) {
            fs.copyFileSync(cachedMod64Path, modVersionCompiled64FilePath);
        }

        if (cachedModArm64Path) {
            fs.copyFileSync(cachedModArm64Path, modVersionCompiledArm64FilePath);
        }
    } else {
        compileMod(modVersionFilePath, modVersionCompiled32FilePath, modVersionCompiled64FilePath, modVersionCompiledArm64FilePath);
    }
}

function generateChangelogEntry(modId: string, commit: string, lastCommit: string, metadata: { version: string }, cache: GitCache) {
    const meta = cache.getCommitMeta(commit);
    const commitTime = meta.timestamp;

    const commitFormattedDate = new Date(commitTime * 1000)
        .toLocaleDateString('en-US', { year: 'numeric', month: 'short', day: 'numeric' });

    const modVersionUrl = `https://github.com/ramensoftware/windhawk-mods/blob/${commit}/mods/${modId}.wh.cpp`;

    let changelogEntry = `## ${metadata.version} ([${commitFormattedDate}](${modVersionUrl}))\n\n`;

    if (commit !== lastCommit) {
        const changelogItem = getModChangelogTextForVersion(modId, metadata.version, meta.message);
        changelogEntry += `${changelogItem}\n\n`;
    } else {
        changelogEntry += 'Initial release.\n';
    }

    return { changelogEntry, commitTime };
}

function generateModData(modId: string, changelogPath: string, modDir: string, modAuthorData: Record<string, ModAuthorData>, cache: GitCache) {
    if (!fs.existsSync(modDir)) {
        fs.mkdirSync(modDir);
    }

    let changelog = '';
    const versions: {
        version: string;
        timestamp: number;
        prerelease?: boolean;
    }[] = [];
    let sawReleaseVersion = false;

    const modSourceUtils = new ModSourceUtils('mods');

    const commits = cache.getModCommits(modId).map(e => e.commit);
    const lastCommit = commits[commits.length - 1];

    for (const commit of commits) {
        const modFile = cache.getBlob(`${commit}:mods/${modId}.wh.cpp`);

        const metadata = modSourceUtils.extractMetadata(modFile, 'en-US');

        if (!metadata.version || !metadata.github || !metadata.author) {
            throw new Error(`Mod ${modId} has incomplete metadata in commit ${commit}`);
        }

        validateAndUpdateAuthorData(modId, commit, {
            github: metadata.github,
            author: metadata.author,
            homepage: metadata.homepage,
            twitter: metadata.twitter,
        }, modAuthorData);

        const prerelease = metadata.version.includes('-');
        if (prerelease && sawReleaseVersion) {
            continue;
        }

        const modVersionFilePath = path.join(modDir, `${metadata.version}.wh.cpp`);
        if (fs.existsSync(modVersionFilePath)) {
            throw new Error(`Mod ${modId} has duplicate version ${metadata.version} in commit ${commit}`);
        }

        fs.writeFileSync(modVersionFilePath, modFile);

        if (!prerelease && !sawReleaseVersion) {
            // Override root file with the latest release version.
            fs.copyFileSync(path.join('mods', `${modId}.wh.cpp`), modVersionFilePath);
            sawReleaseVersion = true;
        }

        handleCompiledFiles(modId, modDir, modVersionFilePath, {
            version: metadata.version,
            architecture: metadata.architecture,
        });

        const { changelogEntry, commitTime } = generateChangelogEntry(modId, commit, lastCommit, {
            version: metadata.version,
        }, cache);
        changelog += changelogEntry;

        versions.unshift({
            version: metadata.version,
            timestamp: commitTime,
            ...(prerelease ? { prerelease: true } : {}),
        });
    }

    fs.writeFileSync(changelogPath, changelog);

    const versionsPath = path.join(modDir, 'versions.json');
    fs.writeFileSync(versionsPath, JSON.stringify(versions));
}

function validateModAuthorData(modAuthorData: Record<string, ModAuthorData>) {
    const seenGithub = new Map<string, string>();
    const seenAuthor = new Map<string, string>();
    const seenHomepage = new Map<string, string>();
    const seenTwitter = new Map<string, string>();

    for (const [authorKey, data] of Object.entries(modAuthorData)) {
        const githubLower = data.github.toLowerCase();
        if (seenGithub.has(githubLower) && seenGithub.get(githubLower) !== authorKey) {
            throw new Error(`Duplicate github '${data.github}' found for authors '${authorKey}' and '${seenGithub.get(githubLower)}'`);
        }
        seenGithub.set(githubLower, authorKey);

        const authorLower = data.author.toLowerCase();
        if (seenAuthor.has(authorLower) && seenAuthor.get(authorLower) !== authorKey) {
            throw new Error(`Duplicate author name '${data.author}' found for authors '${authorKey}' and '${seenAuthor.get(authorLower)}'`);
        }
        seenAuthor.set(authorLower, authorKey);

        for (const homepage of data.homepages) {
            const homepageLower = homepage.toLowerCase();
            if (seenHomepage.has(homepageLower) && seenHomepage.get(homepageLower) !== authorKey) {
                throw new Error(`Duplicate homepage '${homepage}' found for authors '${authorKey}' and '${seenHomepage.get(homepageLower)}'`);
            }
            seenHomepage.set(homepageLower, authorKey);
        }

        if (data.twitter) {
            const twitterLower = data.twitter.toLowerCase();
            if (seenTwitter.has(twitterLower) && seenTwitter.get(twitterLower) !== authorKey) {
                throw new Error(`Duplicate twitter '${data.twitter}' found for authors '${authorKey}' and '${seenTwitter.get(twitterLower)}'`);
            }
            seenTwitter.set(twitterLower, authorKey);
        }
    }
}

function generateModsData(cache: GitCache) {
    const changelogDir = 'changelogs';
    if (!fs.existsSync(changelogDir)) {
        fs.mkdirSync(changelogDir);
    }

    const modAuthorData: Record<string, ModAuthorData> = {};

    const modsSourceDir = fs.opendirSync('mods');
    try {
        let modsSourceDirEntry: fs.Dirent | null;
        while ((modsSourceDirEntry = modsSourceDir.readSync()) !== null) {
            if (modsSourceDirEntry.isFile() && modsSourceDirEntry.name.endsWith('.wh.cpp')) {
                const modId = modsSourceDirEntry.name.slice(0, -'.wh.cpp'.length);
                const changelogPath = path.join(changelogDir, `${modId}.md`);
                const modDir = path.join('mods', modId);
                generateModData(modId, changelogPath, modDir, modAuthorData, cache);
            }
        }
    } finally {
        modsSourceDir.closeSync();
    }

    validateModAuthorData(modAuthorData);

    fs.writeFileSync('mod_author_data.json', JSONstringifyOrder(modAuthorData, 4));
}

function enrichCatalog(catalog: Record<string, any>, enrichment: any, modTimes: any, cache: GitCache) {
    const app = {
        version: enrichment.app.version,
        versionBleedingEdge: enrichment.app.versionBleedingEdge,
    };

    const mods: Record<string, any> = {};
    for (const [id, metadata] of Object.entries(catalog)) {
        const { id: idFromMetadata, ...rest } = metadata;
        if (id !== idFromMetadata) {
            throw new Error(`Expected ${id} === ${idFromMetadata}`);
        }

        modTimes[id] = modTimes[id] || {
            published: getModCreatedTime(id, cache),
            updated: getModModifiedTime(id, cache),
        };

        mods[id] = {
            metadata: rest,
            details: {
                published: modTimes[id].published,
                updated: modTimes[id].updated,
                defaultSorting: 0,
                rating: 0,
                users: 0,
                ratingUsers: 0,
                ratingBreakdown: [0, 0, 0, 0, 0],
                ...enrichment.mods[id]?.details,
            },
        };

        if (enrichment.mods[id]?.featured) {
            mods[id].featured = true;
        }
    }

    return {
        app,
        mods,
    };
}

async function generateModCatalogs(cache: GitCache) {
    const enrichmentUrl = 'https://update.windhawk.net/mods_catalog_enrichment.json';
    const enrichment = await fetchJson(enrichmentUrl);

    const translateFilesUrl = 'https://api.github.com/repos/ramensoftware/windhawk-translate/contents';
    const translateFiles = await fetchJson(translateFilesUrl, {
        Authorization: `Bearer ${process.env.GITHUB_TOKEN}`,
    });

    const modSourceUtils = new ModSourceUtils('mods');

    const modTimes = {};

    const catalog = modSourceUtils.getMetadataOfMods('en-US');
    const catalogEnriched = enrichCatalog(catalog, enrichment, modTimes, cache);
    fs.writeFileSync('catalog.json', JSONstringifyOrder(catalogEnriched, 4));

    const catalogsDir = 'catalogs';
    if (!fs.existsSync(catalogsDir)) {
        fs.mkdirSync(catalogsDir);
    }

    for (const translateFile of translateFiles) {
        const translateFileName = translateFile.name;
        if (!translateFileName.endsWith('.yml')) {
            continue;
        }

        const language = translateFileName.slice(0, -'.yml'.length);
        const catalog = modSourceUtils.getMetadataOfMods(language);
        const catalogEnriched = enrichCatalog(catalog, enrichment, modTimes, cache);
        fs.writeFileSync(path.join(catalogsDir, `${language}.json`), JSONstringifyOrder(catalogEnriched, 4));
    }
}

function getModChangelogTextForVersion(modId: string, modVersion: string, commitMessage: string) {
    const overridePath = path.join('changelog_override', modId, `${modVersion}.md`);
    if (fs.existsSync(overridePath)) {
        return fs.readFileSync(overridePath, 'utf8').trim();
    }

    let messageTrimmed = commitMessage.trim();
    if (messageTrimmed.includes('\n')) {
        // Remove first line.
        return messageTrimmed.replace(/^.* \(#\d+\)\n\n/, '').trim();
    } else {
        // Only remove trailing PR number if it's the only line.
        return messageTrimmed.replace(/ \(#\d+\)$/, '').trim();
    }
}

function generateRssFeed(feedType: 'updates' | 'releases', cache: GitCache) {
    type FeedItem = {
        commit: string;
        title: string;
        content: string;
        url: string;
        date: Date;
        author: {
            name?: string;
            link?: string;
        };
    };

    let feedItems: FeedItem[] = [];

    const modSourceUtils = new ModSourceUtils('mods');

    const allowedChangeTypes = feedType === 'releases' ? ['A'] : ['A', 'M'];

    for (const commit of cache.commitOrder) {
        const files = cache.commitFiles.get(commit);
        if (!files || files.length !== 1) {
            continue;
        }

        const { changeType, filePath } = files[0];
        if (!allowedChangeTypes.includes(changeType)) {
            continue;
        }

        const match = filePath.match(/^mods\/(.+)\.wh\.cpp$/);
        if (!match) {
            continue;
        }

        const modId = match[1];

        const modFile = cache.getBlob(`${commit}:mods/${modId}.wh.cpp`);

        const metadata = modSourceUtils.extractMetadata(modFile, 'en-US');

        if (!metadata.version) {
            throw new Error(`Mod ${modId} has no version in commit ${commit}`);
        }

        const meta = cache.getCommitMeta(commit);
        const commitTime = meta.timestamp;

        let content = '';
        if (changeType === 'M') {
            content = getModChangelogTextForVersion(modId, metadata.version, meta.message);
        } else {
            content = modSourceUtils.extractReadme(modFile) || 'Initial release.';
        }

        feedItems.push({
            commit,
            title: `${metadata.name} ${metadata.version}`,
            content,
            url: `https://windhawk.net/mods/${modId}`,
            date: new Date(commitTime * 1000),
            author: {
                name: metadata.author,
                link: metadata.github,
            },
        });

        if (feedItems.length >= 20) {
            break;
        }
    }

    const feed = new Feed({
        title: feedType === 'releases'
            ? 'Windhawk New Mod Releases'
            : 'Windhawk Mod Updates',
        description: feedType === 'releases'
            ? 'New mods in the official collection of Windhawk mods'
            : 'Updates in the official collection of Windhawk mods',
        id: 'https://windhawk.net/',
        link: 'https://windhawk.net/',
        favicon: 'https://windhawk.net/favicon.ico',
        copyright: 'Ramen Software',
        updated: feedItems[0].date,
    });

    const showdownConverter = new showdown.Converter();

    const markdownToHtml = (markdown: string) => {
        // Showdown doesn't support trailing backslashes as newlines. Use double
        // spaces instead. https://github.com/showdownjs/showdown/issues/394
        markdown = markdown.replace(/\\\n/g, '  \n');

        return showdownConverter.makeHtml(markdown);
    }

    for (const feedItem of feedItems) {
        feed.addItem({
            title: feedItem.title,
            id: feedItem.url + '#' + feedItem.commit,
            link: feedItem.url,
            content: markdownToHtml(feedItem.content),
            date: feedItem.date,
            author: [feedItem.author],
        });
    }

    return feed.atom1();
}

async function main() {
    const cache = buildGitCache();

    generateModsData(cache);

    await generateModCatalogs(cache);

    fs.writeFileSync('updates.atom', generateRssFeed('updates', cache));
    fs.writeFileSync('releases.atom', generateRssFeed('releases', cache));

    const srcPath = 'public';
    for (const file of fs.readdirSync(srcPath, { withFileTypes: true })) {
        fs.renameSync(path.join(srcPath, file.name), file.name);
    }
}

main();
