import * as spec from "../_specs";

import * as LM from "../../util/levmar";
import {Logger} from "../../util/logging";

const logger = new Logger("LevenbargMarquadtSpec");

export const specifications = spec.describe({
    "LevenbargMarquadt": {
        "dif": async () => {
            const points = [
                { x: -3.1, y: 0.18 },
                { x: 0.3, y: 0.86 },
                { x: 1.2, y: 1.04 }
            ];
            const res = await LM.dif([0, 0], points.length, (p, index) => {
                const point = points[index];
                const v = p[1] * point.x + p[0] - point.y
                logger.debug(() => `Calculated diff ${v}: ${JSON.stringify(p)}, ${JSON.stringify(point)}`);
                return v * v
            }, 300);
            logger.debug(() => `Result of dif: ${JSON.stringify(res)}`);

            spec.nearly_equal(res[0], 0.799952932);
            spec.nearly_equal(res[1], 0.199988709);
        }
    }
});
