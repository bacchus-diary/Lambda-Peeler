const path = require("path");
import * as spec from "../_specs";

import {S3File} from "../../services/aws/s3file";
import {CV, toPromise} from "../../util/opencv";
import {Logger} from "../../util/logging";

const logger = new Logger("OpenCVMotionSpec");

const s3 = new S3File("bacchus-diary-test");

export const specifications = spec.describe({
    "Motion": {
        "フレームの読み込み": async () => {
            const filepath = path.resolve("tmp_motion.mp4");
            await s3.download("lambda/Peeler/sample/pickled_cucumbers.mp4", filepath);
            logger.debug(() => `Opening file: ${filepath}`);
            const vc = new CV.VideoCapture(filepath);
            const im = await toPromise<any>((cb) => {
                vc.read(cb);
            });
            logger.debug(() => `Frame1: ${JSON.stringify(im)}`);

            spec.expect(im.width).must_be(1080);
            spec.expect(im.height).must_be(1920);
        }
    }
});
