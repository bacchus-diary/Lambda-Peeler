import * as spec from './_specs';

import {Main} from '../index';
import {Logger} from '../util/logging';

const logger = new Logger('SampleSpec');

export const specifications = spec.describe({
    'SampleText': {
        'サンプルは同じ': () => {
            const main = new Main('AA1');
            logger.debug(() => `Main: ${main.sample}`);
            spec.expect(main.sample).must_be('AA1');
        }
    }
});
