import {Logger} from "../../util/logging";

export * from "aws-sdk";

const logger = new Logger("AWS");

export interface Request {
    send(callback: (err, data) => void): void;
}

export function requestToPromise<R>(request: Request, title?: string): Promise<R> {
    return new Promise<R>((resolve, reject) => {
        request.send((err, data) => {
            if (err) {
                if (title) logger.warn(() => `${title}: Failed to call: ${err}`);
                reject(err);
            } else {
                if (title) logger.debug(() => `${title}: Result: ${JSON.stringify(data)}`);
                resolve(data);
            }
        });
    });
}
