import * as spec from "../_specs";

import {Logger} from "../../util/logging";

const cv = require("opencv");

const logger = new Logger("OpenCVSampleSpec");

type OpenCVCallback<T> = (error, data: T) => void;
function toPromise<T>(proc: (cb: OpenCVCallback<T>) => void): Promise<T> {
    return new Promise((resolve, reject) => {
        proc((error, data) => {
            if (error) reject(error);
            else resolve(data);
        });
    });
}

export const specifications = spec.describe({
    "MonaLisa": {
        "顔の位置": async () => {
            const im = await toPromise<any>((cb) => {
                cv.readImage("./node_modules/opencv/examples/files/mona.png", cb);
            });
            if (im.width() < 1 || im.height() < 1) throw "Image has no size";
            const faces = await toPromise<any>((cb) => {
                im.detectObject("./node_modules/opencv/data/haarcascade_frontalface_alt.xml", {}, cb);
            });
            logger.debug(() => `Faces: ${JSON.stringify(faces)}`);

            spec.expect(faces.length).must_be(1);
            spec.expect(faces[0]).must_be({
                x: 155,
                y: 117,
                width: 140,
                height: 140
            });
        }
    }
});
