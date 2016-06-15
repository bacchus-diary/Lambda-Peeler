import * as spec from "../_specs";
import {Logger} from "../../util/logging";

const ffi = require("ffi");
const ref = require("ref");

const logger = new Logger("HaskellSpec");

const C_string = ref.types.CString;

const peeler = ffi.Library("libHSPeeler", {
    "start": [C_string, [C_string]]
});

export const specifications = spec.describe({
    "Haskell": {
        "start": () => {
            const original = "Some text";
            const result = peeler.start(original);
            logger.debug(() => `Result of dif: ${JSON.stringify(result)}`);

            spec.expect(result).must_be(original);
        }
    }
});
