import {Main} from '../src/index';

describe('SampleText', () => {
    it('サンプルは同じ', () => {
        const main = new Main('AA1');
        expect(main.sample).toBe('AA1');
    });
});
