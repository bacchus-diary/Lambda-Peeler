import cp = require('child_process');
import {Logger} from './util/logging';

const logger = new Logger('Main');

export class Main {
    static run() {
        console.log('Hello World !');
    }

    constructor(private _sample: string) { }

    get sample(): string {
        return this._sample;
    }
}

export function handler(event, context, callback) {
    logger.info(() => `Handling event: ${JSON.stringify(event)}`);
    if (event == 'TEST') {
        logger.info(() => `Test run...`);
        cp.exec('./node_modules/jasmine/bin/jasmine.js', (error, stdout, stderr) => {
            console.log(error);
            console.log(stdout);
            console.log(stderr);
            callback(error, null);
        });
    } else {
        const error = null;
        const result = 'OK';
        callback(error, result);
    }
}
logger.info(() => `Loading me ...`);
