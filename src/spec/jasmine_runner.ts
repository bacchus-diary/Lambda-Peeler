import cp = require('child_process');
import {Logger} from '../util/logging';

const logger = new Logger('JasmineRunner');

export function jasmine(callback: (error: any) => void) {
    logger.info(() => `Test run...`);
    process.env.JASMINE_CONFIG_PATH = 'jasmine.json'
    cp.exec('./node_modules/jasmine/bin/jasmine.js', (error, stdout, stderr) => {
        if (stdout) console.log(stdout);
        if (stderr) console.log(stderr);
        callback(error);
    });
}
