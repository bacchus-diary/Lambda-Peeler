import cp = require('child_process');
import {Logger} from '../util/logging';

const logger = new Logger('JasmineRunner');

export function jasmine(callback: (error: any) => void) {
    logger.info(() => `Test run...`);
    process.env.JASMINE_CONFIG_PATH = 'jasmine.json'
    const p = cp.spawn('./node_modules/jasmine/bin/jasmine.js', [], { shell: false, stdio: 'inherit'});
    p.on('close', (code) => {
        callback(code == 0 ? null : `Process result code:${code}`);
    });
}
