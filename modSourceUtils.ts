import * as fs from 'fs';
import * as path from 'path';

const modMetadataParams = {
	singleValue: [
		'id',
		'version',
		'github',
		'twitter',
		'homepage',
		'compilerOptions',
		'license',
		'donateUrl',
	],
	singleValueLocalizable: [
		'name',
		'description',
		'author',
	],
	multiValue: [
		'include',
		'exclude',
		'architecture',
	],
} as const;

type ModMetadataParamsSingleValue = typeof modMetadataParams.singleValue[number];
type ModMetadataParamsSingleValueLocalizable = typeof modMetadataParams.singleValueLocalizable[number];
type ModMetadataParamsMultiValue = typeof modMetadataParams.multiValue[number];

function isModMetadataParamsSingleValue(k: string): k is ModMetadataParamsSingleValue {
	return modMetadataParams.singleValue.includes(k as any);
}
function isModMetadataParamsSingleValueLocalizable(k: string): k is ModMetadataParamsSingleValueLocalizable {
	return modMetadataParams.singleValueLocalizable.includes(k as any);
}
function isModMetadataParamsMultiValue(k: string): k is ModMetadataParamsMultiValue {
	return modMetadataParams.multiValue.includes(k as any);
}

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

			const match = lineTrimmed.match(/^\/\/[ \t]+@(_?[a-zA-Z]+)(?::([a-z]{2}(?:-[A-Z]{2})?))?[ \t]+(.*)$/);
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
			'x86-64',
			'amd64',
			'arm64'
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

		for (const [metadataKeyRaw, metadataValue] of Object.entries(metadataRaw)) {
			if (metadataValue.length === 0) {
				throw new Error(`Missing metadata parameter: ${metadataKeyRaw}`);
			}

			const metadataKey = metadataKeyRaw.replace(/^_/, '');

			if (isModMetadataParamsSingleValueLocalizable(metadataKey)) {
				const languages = new Set<string | null>();
				for (const item of metadataValue) {
					if (languages.has(item.language)) {
						throw new Error(`Duplicate metadata parameter: ${metadataKey}` + (item.language !== null ? `:${item.language}` : ''));
					}

					languages.add(item.language);
				}

				result[metadataKey] = this.getBestLanguageMatch(language, metadataValue).value;
			} else if (isModMetadataParamsMultiValue(metadataKey)) {
				for (const item of metadataValue) {
					if (item.language !== null) {
						throw new Error(`Metadata parameter can't be localized: ${metadataKey}:${item.language}`);
					}
				}

				result[metadataKey] = metadataValue.map(x => x.value);
			} else if (isModMetadataParamsSingleValue(metadataKey)) {
				for (const item of metadataValue) {
					if (item.language !== null) {
						throw new Error(`Metadata parameter can't be localized: ${metadataKey}:${item.language}`);
					}
				}

				if (metadataValue.length > 1) {
					throw new Error(`Duplicate metadata parameter: ${metadataKey}`);
				}

				result[metadataKey] = metadataValue[0].value;
			} else if (metadataKeyRaw.startsWith('_')) {
				// Ignore for forward compatibility.
			} else {
				throw new Error(`Unsupported metadata parameter: ${metadataKey}`);
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
