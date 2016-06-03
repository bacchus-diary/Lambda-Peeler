import * as spec from "../_specs";

import * as LM from "../../util/levmar";
import {Logger} from "../../util/logging";

const logger = new Logger("LevenbargMarquadtSpec");

export const specifications = spec.describe({
    "LevenbargMarquadt": {
        "dif": async () => {
            const points = [
              {x: -3.1, y: 0.18},
              {x: 0.3, y: 0.86},
              {x: 1.2, y: 1.04}
            ];
            const res = await LM.dif([0, 0], points.length, (p, index) => {
              const point = points[index];
              logger.debug(() => `Calculating ${JSON.stringify(p)} from ${JSON.stringify(point)}`);
              const v = p[1] * point.x + p[0] - point.y
              return v * v
            });
            logger.debug(() => `Result of dif: ${JSON.stringify(res)}`);

            spec.expect(res[0]).must_be(0.806997);
            spec.expect(res[1]).must_be(0.188845);
        }
    }
});
