import * as spec from "./spec/_specs";
import {Logger} from "./util/logging";

const logger = new Logger("Hander");

export function handler(event, context, callback) {
    logger.info(() => `Handling event: ${JSON.stringify(event)}`);
    try {
        if (event === "TEST") {
            spec.run((err) => {
                callback(err, "Done test");
            });
        } else {
            const error = null;
            const result = "OK";
            callback(error, result);
        }
    } catch (ex) {
        callback(ex, null);
    }
}
logger.info(() => `Loading me ...`);
