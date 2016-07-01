import * as spec from "../_specs";
import {S3File} from "../../services/aws/s3file";
import {Logger} from "../../util/logging";

const path = require("path");

const logger = new Logger("HaskellSpec");

const s3 = new S3File("build-config");

const proc = require('child_process');
function exec(cmd) {
    return new Promise((resolve, reject) => {
        logger.debug(() => `Executing: ${cmd}`);
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

export const specifications = spec.describe({
    "Haskell": {
        "start": async () => {
            const s3path = "bacchus-diary/Lambda-Peeler/test_data/pickled_cucumbers.mp4";
            const filepath = path.resolve(`/tmp/${path.basename(s3path)}`);
            await s3.download(s3path, filepath);

            await exec(`./haskell/bin/peeler ${filepath}`);
        }
    }
});
