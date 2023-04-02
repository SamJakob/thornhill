import { fileURLToPath } from 'url';

/**
 * Locates the base directory of the entire project.
 */
export function locateProjectBaseDirectory() {
    return fileURLToPath(import.meta.url);
}

/**
 * Trivially, scans the current working directory for the directory
 * used as the cmake directory. (Typically, cmake-build-debug).
 */
function scanForCMakeDirectory() {
}