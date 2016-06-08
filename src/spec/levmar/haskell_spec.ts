import * as spec from "../_specs";
import * as LM from "../../util/levmar";
import {Logger} from "../../util/logging";

const ffi = require("ffi");
const ref = require("ref");

const logger = new Logger("HaskellSpec");

const C_double = ref.types.double;
const C_doublePtr = ref.refType(C_double);

const peeler = ffi.Library("libHSPeeler", {
    "calcSin": [C_double, [C_double]]
});

export const specifications = spec.describe({
    "Haskell": {
        "sin": async () => {
            const result = peeler.calcSin(0.5);
            logger.debug(() => `Result of dif: ${JSON.stringify(result)}`);

            spec.nearly_equal(result, 0.479425539);
        }
    }
});
