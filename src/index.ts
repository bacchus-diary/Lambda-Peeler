import * as spec from './spec/_specs';
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

    async flight(): Promise<string> {
        return new Promise<string>((resolve, reject) => {
            logger.info(() => `Taking off...`);
            setTimeout(() => {
                logger.info(() => `Flighting`);
                resolve('Sky');
            }, 1000);
        });
    }
}

export function handler(event, context, callback) {
    logger.info(() => `Handling event: ${JSON.stringify(event)}`);
    try {
        if (event == 'TEST') {
            spec.run((err) => {
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
