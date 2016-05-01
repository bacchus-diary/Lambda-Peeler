import * as spec from '../_specs';

import * as cv from 'opencv';

import {Logger} from '../../util/logging';

const logger = new Logger('OpenCVSampleSpec');

export const specifications = spec.describe({
    'MonaLisa': {
        '顔の位置': async () => {
            const im = await new Promise<any>((resolve, reject) => {
                cv.readImage('./node_modules/opencv/examples/files/mona.png', function(err, im) {
                    if (err) reject(err);
                    else {
                        if (im.width() < 1 || im.height() < 1) reject('Image has no size');
                        else resolve(im);
                    }
                })
            });
            const faces = await new Promise<any>((resolve, reject) => {
                im.detectObject('./node_modules/opencv/data/haarcascade_frontalface_alt.xml', {}, function(err, faces) {
                    if (err) reject(err);
                    else resolve(faces);
                });
            });
            logger.debug(() => `Faces: ${JSON.stringify(faces)}`);

            spec.expect(faces.length).must_be(1);
            spec.expect(faces[0].x).must_be(155);
            spec.expect(faces[0].y).must_be(117);
            spec.expect(faces[0].width).must_be(140);
            spec.expect(faces[0].height).must_be(140);
        }
    }
});
