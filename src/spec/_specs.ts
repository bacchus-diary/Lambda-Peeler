import {Logger} from '../util/logging';

const logger = new Logger('SpecRunner');

export function run(callback: (error?: any) => void) {
    logger.info(() => `Test start...`);
    all().then((failures) => {
        if (_.isEmpty(failures)) {
            logger.info(() => `Test finished successfully.`);
            callback();
        } else {
            logger.warn(() => `Test failed: ${JSON.stringify(failures, null, 4)}`);
            callback(failures);
        }
    }).catch((ex) => {
        callback(ex);
    })
}

async function all(): Promise<{[key: string]: SpecResult}> {
    async function spec(path: string) {
        const mod = require(path);
        const result = await mod.specifications();
        return _.isEmpty(result) ? null : [path, result];
    }
    return _.fromPairs(_.compact(await Promise.all([
        /* __FATHENS_ALL_SPECS__ */
    ])));
}

export type Specifications = () => Promise<SpecResult>;

export type SpecResult = { [key: string]: DescribeResult };

export type DescribeResult = { [key: string]: TestFailure };

export type TestFailure = {
    name: string,
    description: string,
    exception: string
}

export type SpecUnit = () => void | Promise<void>;

export function describe(specs: { [key: string]: { [key: string]: SpecUnit } }): Specifications {
    return async () => {
        const result: SpecResult = {};
        await Promise.all(_.keys(specs).map(async (name) => {
            const descs = specs[name];
            const failures: DescribeResult = {};
            await Promise.all(_.keys(descs).map(async (desc) => {
                try {
                    await descs[desc]();
                } catch (ex) {
                    failures[desc] = {
                        name: name,
                        description: desc,
                        exception: ex.toString()
                    };
                }
            }));
            if (!_.isEmpty(failures)) result[name] = failures;
        }))
        return result;
    }
}

export function expect<T>(v: T): Expect<T> {
    return new Expect(v);
}

export class Expect<T> {
    constructor(private value: T) { }

    must_be(expected: T) {
        if (this.value != expected) {
            throw `'${this.value}' must be '${expected}'`;
        }
    }
}
