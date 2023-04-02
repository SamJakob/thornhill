// Import build configuration
import config from '../../thornhill-build-config.json' assert { type: 'json' };

import utilities from './utilities.mjs';

// Begin script

/**
 * Run the hot-reload toolchain/
 */
function main() {
    console.log(config);
}

(async () => {
    main();
})();