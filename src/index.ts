import {jasmine} from './spec/jasmine_runner';
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
    try {
        if (event == 'TEST') {
            jasmine((err) => {
                callback(err, 'Done test');
            });
        } else {
            const error = null;
            const result = 'OK';
            callback(error, result);
        }
    } catch (ex) {
        callback(ex, null);
    }
}
logger.info(() => `Loading me ...`);
