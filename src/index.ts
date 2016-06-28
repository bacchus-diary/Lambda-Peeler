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
        } else if (event.mission === "CHECK") {
            checkEnv(event.commands, (err) => {
                callback(err, "Done check")
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

function checkEnv(commands, fin) {
    commands.reduce(
        (pre, cmd) => pre.then(
            () => exec(cmd)
        ), Promise.resolve()
    ).then(fin);
}

const proc = require('child_process');
function exec(cmd) {
    return new Promise((resolve, reject) => {
        proc.exec(cmd, (error, stdout, stderr) => {
            logger.info(() => `${cmd}: STDOUT: ${stdout}`);
            logger.warn(() => `${cmd}: STDERR: ${stderr}`);
            if (!_.isNil(error)) {
                logger.warn(() => `${cmd}: ERROR: ${error}`);
            }
            resolve();
        });
    });
}
