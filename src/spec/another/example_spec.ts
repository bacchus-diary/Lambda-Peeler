import * as spec from '../_specs';

import {Main} from '../../index';
import {Logger} from '../../util/logging';

const logger = new Logger('ExampleSpec');

export const specifications = spec.describe({
    'SampleText': {
        '空に飛ぶまでは1秒かかる': async () => {
            const start = new Date().getTime();
            const sky = await new Main('a').flight();
            const end = new Date().getTime();
            spec.expect(sky).must_be('Sky');
            logger.debug(() => `Flight start: ${start}, end: ${end}`);
            spec.nearly_equal(end - start, 1000, 0.01);
        }
    }
});
