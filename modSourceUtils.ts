import * as fs from 'fs';
import * as path from 'path';

const modMetadataParams = {
	singleValue: {
		'id': true,
		'version': true,
		'github': true,
		'twitter': true,
		'homepage': true,
		'compilerOptions': true,
	},
	singleValueLocalizable: {
		'name': true,
		'description': true,
		'author': true,
	},
	multiValue: {
		'include': true,
		'exclude': true,
		'architecture': true,
	}
};

type ModMetadataParamsSingleValue = keyof typeof modMetadataParams.singleValue;
type ModMetadataParamsSingleValueLocalizable = keyof typeof modMetadataParams.singleValueLocalizable;
type ModMetadataParamsMultiValue = keyof typeof modMetadataParams.multiValue;

type ModMetadata = Partial<
	Record<ModMetadataParamsSingleValue, string> &
	Record<ModMetadataParamsSingleValueLocalizable, string> &
	Record<ModMetadataParamsMultiValue, string[]>
>;

export default class ModSourceUtils {
	private modsSourcePath: string;

	public constructor(modsSourcePath: string) {
		this.modsSourcePath = modsSourcePath;
	}

	private getBestLanguageMatch(matchLanguage: string, candidates: {
		language: string | null,
		value: string
	}[]) {
		const languages = candidates.map(x => x.language && x.language.toLowerCase());

		let iterLanguage = matchLanguage;
		let foundIndex;

		for (;;) {
			// Exact match.
			foundIndex = languages.indexOf(iterLanguage);
			if (foundIndex !== -1) {
				return candidates[foundIndex];
			}

			// A more specific language.
			foundIndex = languages.findIndex(language => language && language.startsWith(iterLanguage));
			if (foundIndex !== -1) {
				return candidates[foundIndex];
			}

			if (!iterLanguage.includes('-')) {
				break;
			}

			iterLanguage = iterLanguage.replace(/-[^-]*$/, '');
		}

		// No language.
		foundIndex = languages.indexOf(null);
		if (foundIndex !== -1) {
			return candidates[foundIndex];
		}

		// No matches of any kind, return the first item.
		return candidates[0];
	}

	private extractMetadataRaw(modSource: string) {
		const metadataBlockMatch = modSource.match(/^\/\/[ \t]+==WindhawkMod==[ \t]*$([\s\S]+?)^\/\/[ \t]+==\/WindhawkMod==[ \t]*$/m);
		if (!metadataBlockMatch) {
			throw new Error('Couldn\'t find a metadata block in the source code');
		}

		const metadataBlock = metadataBlockMatch[1];

		const result: Record<string, {
			language: string | null,
			value: string
		}[]> = {};

		for (const line of metadataBlock.split('\n')) {
			const lineTrimmed = line.trimEnd();
			if (lineTrimmed === '') {
				continue;
			}

			const match = lineTrimmed.match(/^\/\/[ \t]+@([a-zA-Z]+)(?::([a-z]{2}(?:-[A-Z]{2})?))?[ \t]+(.*)$/);
			if (!match) {
				const lineTruncated = lineTrimmed.length > 20 ? (lineTrimmed.slice(0, 17) + '...') : lineTrimmed;
				throw new Error('Couldn\'t parse metadata line: ' + lineTruncated);
			}

			const key = match[1];
			const language = match[2] as string | undefined;
			const value = match[3];

			result[key] = result[key] ?? [];
			result[key].push({
				language: language ?? null,
				value
			});
		}

		return result;
	}

	private validateMetadata(metadata: ModMetadata) {
		const modId = metadata.id;
		if (!modId) {
			throw new Error('Mod id must be specified in the source code');
		}

		if (!modId.match(/^[0-9a-z-]+$/)) {
			throw new Error('Mod id must only contain the following characters: 0-9, a-z, and a hyphen (-)');
		}

		const paths = {
			include: metadata.include,
			exclude: metadata.exclude
		};
		for (const [category, pathsArray] of Object.entries(paths)) {
			for (const path of pathsArray || []) {
				if (path.match(/[/"<>|]/)) {
					throw new Error(`Mod ${category} path contains one of the forbidden characters: / " < > |`);
				}
			}
		}

		const supportedArchitecture = [
			'x86',
			'x86-64'
		];
		for (const architecture of metadata.architecture || []) {
			if (!supportedArchitecture.includes(architecture)) {
				throw new Error(`Mod architecture must be one of ${supportedArchitecture.join(', ')}: ${architecture}`);
			}
		}
	}

	public extractMetadata(modSource: string, language: string) {
		const metadataRaw = this.extractMetadataRaw(modSource);

		const result: ModMetadata = {};

		for (const [metadataKey, metadataValue] of Object.entries(metadataRaw)) {
			let localizable = false;
			let array = false;

			if (metadataKey in modMetadataParams.singleValue) {
				// Default params.
			} else if (metadataKey in modMetadataParams.singleValueLocalizable) {
				localizable = true;
			} else if (metadataKey in modMetadataParams.multiValue) {
				array = true;
			} else {
				throw new Error(`Unsupported metadata parameter: ${metadataKey}`);
			}

			const candidates: typeof metadataValue = [];
			const languages = new Set<string | null>();

			for (const item of metadataValue) {
				if (localizable) {
					if (languages.has(item.language)) {
						throw new Error(`Duplicate metadata parameter: ${metadataKey}` + (item.language !== null ? `:${item.language}` : ''));
					}

					languages.add(item.language);
				} else if (item.language !== null) {
					throw new Error(`Metadata parameter can't be localized: ${metadataKey}:${item.language}`);
				}

				candidates.push(item);
			}

			if (candidates.length === 0) {
				continue;
			}

			if (localizable) {
				result[metadataKey as ModMetadataParamsSingleValueLocalizable] = this.getBestLanguageMatch(language, candidates).value;
			} else if (array) {
				result[metadataKey as ModMetadataParamsMultiValue] = candidates.map(x => x.value);
			} else {
				if (candidates.length > 1) {
					throw new Error(`Duplicate metadata parameter: ${metadataKey}`);
				}

				result[metadataKey as ModMetadataParamsSingleValue] = candidates[0].value;
			}
		}

		this.validateMetadata(result);
		return result;
	}

	public getMetadataOfMods(language: string) {
		const mods: Record<string, ModMetadata> = {};

		const modsSourceDir = fs.opendirSync(this.modsSourcePath);
		try {
			let modsSourceDirEntry: fs.Dirent | null;
			while ((modsSourceDirEntry = modsSourceDir.readSync()) !== null) {
				if (modsSourceDirEntry.isFile() && modsSourceDirEntry.name.endsWith('.wh.cpp')) {
					const modId = modsSourceDirEntry.name.slice(0, -'.wh.cpp'.length);
					const modSourcePath = path.join(this.modsSourcePath, modsSourceDirEntry.name);
					const modSourceMetadata = this.extractMetadata(fs.readFileSync(modSourcePath, 'utf8'), language);
					mods[modId] = modSourceMetadata;
				}
			}
		} finally {
			modsSourceDir.closeSync();
		}

		return mods;
	}

	public extractReadme(modSource: string) {
		const readmeBlockMatch = modSource.match(/^\/\/[ \t]+==WindhawkModReadme==[ \t]*$\s*\/\*\s*([\s\S]+?)\s*\*\/\s*^\/\/[ \t]+==\/WindhawkModReadme==[ \t]*$/m);
		if (readmeBlockMatch === null) {
			return null;
		}

		return readmeBlockMatch[1];
	}
}
