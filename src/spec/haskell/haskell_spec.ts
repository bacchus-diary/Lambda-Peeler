import * as spec from "../_specs";
import {S3File} from "../../services/aws/s3file";
import {Logger} from "../../util/logging";

const path = require("path");

const logger = new Logger("HaskellSpec");

const s3 = new S3File("build-config");

export const specifications = spec.describe({
    "Haskell": {
        "start": async () => {
            const s3path = "bacchus-diary/Lambda-Peeler/test_data/pickled_cucumbers.mp4";
            const filepath = path.resolve(`/tmp/${path.basename(s3path)}`);
            await s3.download(s3path, filepath);
            logger.debug(() => `Loading video: ${filepath}`);

            await new Promise((resolve, reject) => {
                require("child_process").spawn("./haskell/bin/peeler", [filepath], {stdio: "inherit"}).on("close", (code) => {
                    spec.expect(code).must_be(0);
                    resolve();
                });
            });
        }
    }
});
