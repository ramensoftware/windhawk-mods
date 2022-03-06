import * as child_process from 'child_process';
import * as fs from 'fs';
import * as https from 'https';
import * as path from 'path';
import ModSourceUtils from './modSourceUtils';

// Inspired by https://gist.github.com/ktheory/df3440b01d4b9d3197180d5254d7fb65
async function fetchJson(url: string) {
    return new Promise<any>((resolve, reject) => {
        const req = https.request(url,
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

function getModCreatedTime(modId: string) {
    const command = `git log --diff-filter=A --format=%ct -1 -- mods/${modId}.wh.cpp`;
    const time = parseInt(child_process.execSync(command, { encoding: "utf8" }), 10);
    if (isNaN(time)) {
        throw new Error(`Can't get created time for ${modId}`);
    }

    return time * 1000;
}

function getModModifiedTime(modId: string) {
    const command = `git log --format=%ct -1 -- mods/${modId}.wh.cpp`;
    const time = parseInt(child_process.execSync(command, { encoding: "utf8" }), 10);
    if (isNaN(time)) {
        throw new Error(`Can't get modified time for ${modId}`);
    }

    return time * 1000;
}

async function enrichCatalog(catalog: Record<string, any>) {
    const url = 'https://update.windhawk.net/mods_catalog_enrichment.json';
    const enrichment = await fetchJson(url);

    const app = {
        version: enrichment.app.version,
    };

    const mods: Record<string, any> = {};
    for (const [id, metadata] of Object.entries(catalog)) {
        const { id: idFromMetadata, ...rest } = metadata;
        if (id !== idFromMetadata) {
            throw new Error(`Expected ${id} === ${idFromMetadata}`);
        }

        mods[id] = {
            metadata: rest,
            details: {
                published: getModCreatedTime(id),
                updated: getModModifiedTime(id),
                ...(enrichment.mods[id]?.details || {}),
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

async function generateModCatalog() {
    const modSourceUtils = new ModSourceUtils('mods');
    const catalog = modSourceUtils.getMetadataOfMods('en-US');
    return await enrichCatalog(catalog);
}

async function main() {
    const catalog = await generateModCatalog();
    fs.writeFileSync('catalog.json', JSONstringifyOrder(catalog, 4));

    const srcPath = 'public';
    for (const file of fs.readdirSync(srcPath, { withFileTypes: true })) {
        fs.renameSync(path.join(srcPath, file.name), file.name);
    }
}

main();
