import {Main} from '../index';

describe('SampleText', () => {
    it('サンプルは同じ', () => {
        const main = new Main('AA1');
        expect(main.sample).toBe('AA1');
    });
});
