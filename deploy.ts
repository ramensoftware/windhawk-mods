import * as child_process from 'child_process';
import * as fs from 'fs';
import * as https from 'https';
import * as path from 'path';
import ModSourceUtils from './modSourceUtils';
import { Feed } from 'feed';
import { OutgoingHttpHeaders } from 'http';
import showdown from 'showdown';

type ModAuthorData = {
    github: string;
    author: string;
    homepages: Set<string>,
    twitter?: string;
};

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

function getModCreatedTime(modId: string) {
    const time = parseInt(gitExec([
        'log',
        '--diff-filter=A',
        '--format=%ct',
        '-1',
        '--',
        `mods/${modId}.wh.cpp`,
    ]), 10);
    if (isNaN(time)) {
        throw new Error(`Can't get created time for ${modId}`);
    }

    return time * 1000;
}

function getModModifiedTime(modId: string) {
    const time = parseInt(gitExec([
        'log',
        '--format=%ct',
        '-1',
        '--',
        `mods/${modId}.wh.cpp`,
    ]), 10);
    if (isNaN(time)) {
        throw new Error(`Can't get modified time for ${modId}`);
    }

    return time * 1000;
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
            homepages: new Set<string>(),
        };
    }

    const entry = modAuthorData[authorKey];

    if (metadata.homepage !== undefined) {
        entry.homepages.add(metadata.homepage);
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

function generateChangelogEntry(modId: string, commit: string, lastCommit: string, metadata: { version: string }) {
    const commitTime = parseInt(gitExec([
        'log',
        '--format=%ct',
        '-1',
        commit,
    ]), 10);

    const commitFormattedDate = new Date(commitTime * 1000)
        .toLocaleDateString('en-US', { year: 'numeric', month: 'short', day: 'numeric' });

    const modVersionUrl = `https://github.com/ramensoftware/windhawk-mods/blob/${commit}/mods/${modId}.wh.cpp`;

    let changelogEntry = `## ${metadata.version} ([${commitFormattedDate}](${modVersionUrl}))\n\n`;

    if (commit !== lastCommit) {
        const commitMessage = gitExec([
            'log',
            '-1',
            '--pretty=format:%B',
            commit,
        ]);
        const changelogItem = getModChangelogTextForVersion(modId, metadata.version, commitMessage);
        changelogEntry += `${changelogItem}\n\n`;
    } else {
        changelogEntry += 'Initial release.\n';
    }

    return { changelogEntry, commitTime };
}

function generateModData(modId: string, changelogPath: string, modDir: string, modAuthorData: Record<string, ModAuthorData>) {
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

    const commits = gitExec([
        'rev-list',
        'HEAD',
        '--',
        `mods/${modId}.wh.cpp`,
    ]).trim().split('\n');
    const lastCommit = commits[commits.length - 1];

    for (const commit of commits) {
        const modFile = gitExec([
            'show',
            `${commit}:mods/${modId}.wh.cpp`,
        ]);

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
        });
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

function generateModsData() {
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
                generateModData(modId, changelogPath, modDir, modAuthorData);
            }
        }
    } finally {
        modsSourceDir.closeSync();
    }

    validateModAuthorData(modAuthorData);

    fs.writeFileSync('mod_author_data.json', JSONstringifyOrder(modAuthorData, 4));
}

function enrichCatalog(catalog: Record<string, any>, enrichment: any, modTimes: any) {
    const app = {
        version: enrichment.app.version,
    };

    const mods: Record<string, any> = {};
    for (const [id, metadata] of Object.entries(catalog)) {
        const { id: idFromMetadata, ...rest } = metadata;
        if (id !== idFromMetadata) {
            throw new Error(`Expected ${id} === ${idFromMetadata}`);
        }

        modTimes[id] = modTimes[id] || {
            published: getModCreatedTime(id),
            updated: getModModifiedTime(id),
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

async function generateModCatalogs() {
    const enrichmentUrl = 'https://update.windhawk.net/mods_catalog_enrichment.json';
    const enrichment = await fetchJson(enrichmentUrl);

    const translateFilesUrl = 'https://api.github.com/repos/ramensoftware/windhawk-translate/contents';
    const translateFiles = await fetchJson(translateFilesUrl, {
        Authorization: `Bearer ${process.env.GITHUB_TOKEN}`,
    });

    const modSourceUtils = new ModSourceUtils('mods');

    const modTimes = {};

    const catalog = modSourceUtils.getMetadataOfMods('en-US');
    const catalogEnriched = enrichCatalog(catalog, enrichment, modTimes);
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
        const catalogEnriched = enrichCatalog(catalog, enrichment, modTimes);
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

function generateRssFeed() {
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

    const commits = gitExec([
        'rev-list',
        'HEAD',
    ]).trim().split('\n');

    for (const commit of commits) {
        const changedFiles = gitExec([
            'show',
            '--name-status',
            '--pretty=format:',
            commit,
        ]).trim().split('\n');
        if (changedFiles.length !== 1) {
            continue;
        }

        const [changeType, filePath] = changedFiles[0].split('\t');
        if (changeType !== 'A' && changeType !== 'M') {
            continue;
        }

        const match = filePath.match(/^mods\/(.+)\.wh\.cpp$/);
        if (!match) {
            continue;
        }

        const modId = match[1];

        const modFile = gitExec([
            'show',
            `${commit}:mods/${modId}.wh.cpp`,
        ]);

        const metadata = modSourceUtils.extractMetadata(modFile, 'en-US');

        if (!metadata.version) {
            throw new Error(`Mod ${modId} has no version in commit ${commit}`);
        }

        const commitTime = parseInt(gitExec([
            'log',
            '--format=%ct',
            '-1',
            commit,
        ]), 10);

        let content = '';
        if (changeType === 'M') {
            const commitMessage = gitExec([
                'log',
                '-1',
                '--pretty=format:%B',
                commit,
            ]);
            content = getModChangelogTextForVersion(modId, metadata.version, commitMessage);
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
        title: 'Windhawk Mod Updates',
        description: 'Updates in the official collection of Windhawk mods',
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
    generateModsData();

    await generateModCatalogs();

    fs.writeFileSync('updates.atom', generateRssFeed());

    const srcPath = 'public';
    for (const file of fs.readdirSync(srcPath, { withFileTypes: true })) {
        fs.renameSync(path.join(srcPath, file.name), file.name);
    }
}

main();
