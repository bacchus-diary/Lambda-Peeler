import * as spec from "../_specs";

import {CV, toPromise} from "../../util/opencv";
import {Logger} from "../../util/logging";

const logger = new Logger("OpenCVSampleSpec");

export const specifications = spec.describe({
    "MonaLisa": {
        "顔の位置": async () => {
            const im = await toPromise<any>((cb) => {
                CV.readImage("./node_modules/opencv/examples/files/mona.png", cb);
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
