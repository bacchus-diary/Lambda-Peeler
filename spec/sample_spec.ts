import {Main} from '../src/index';
import {Logger} from '../src/util/logging';

const logger = new Logger('SampleSpec');

describe('SampleText', () => {
    it('サンプルは同じ', () => {
        const main = new Main('AA1');
        logger.debug(() => `Main: ${main}`);
        expect(main.sample).toBe('AA1');
    });
});
