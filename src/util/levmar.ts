const ffi = require("ffi");
const ref = require("ref");
const ArrayType = require('ref-array');

import {Logger} from "./logging";

const logger = new Logger("LevenbargMarquadt");

const C_void = ref.types.void;
const C_voidPtr = ref.refType(C_void);

const C_int = ref.types.int;

const C_double = ref.types.double;
const C_doublePtr = ref.refType(C_double);
const C_doubleArray = ArrayType(C_double);

const liblevmar = ffi.Library("liblevmar", {
    "dlevmar_dif": [C_void, [
        ffi.Function(C_void, [C_doubleArray, C_doubleArray, C_int, C_int, C_voidPtr]),
        C_doubleArray, C_doublePtr, C_int, C_int, C_int, C_doublePtr, C_doublePtr, C_doublePtr, C_doublePtr, C_voidPtr]
    ]
});

function toCArray<T>(array: Array<T>, C_type): any {
    const c_array = new C_type(array.length);
    _.range(array.length).forEach(index => {
        c_array[index] = array[index];
    });
    return c_array;
}

function fromCArray<T>(c_array, length: number): Array<T> {
    const array = new Array<T>(length);
    _.range(array.length).forEach(index => {
        array[index] = c_array[index];
    })
    return array;
}

export async function dif(params: Array<number>, numCalcs: number, calc: (p: Array<number>, index: number) => number, iterateMax: number = 1000): Promise<Array<number>> {
    const C_params = toCArray(params, C_doubleArray);

    return new Promise<Array<number>>((resolve, reject) => {
        liblevmar.dlevmar_dif.async((p: Array<number>, hx: Array<number>, m: number, n: number, adata: Array<any>) => {
            const moving = fromCArray<number>(p, params.length);
            _.range(numCalcs).forEach(index => {
                hx[index] = calc(moving, index);
            });
        }, C_params, null, params.length, numCalcs, iterateMax, null, null, null, null, null, (err, res) => {
            if (_.isNil(err)) {
                resolve(fromCArray<number>(C_params, params.length));
            } else {
                reject(err);
            }
        });
    });
}
