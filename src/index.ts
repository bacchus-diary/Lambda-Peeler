export class Main {
    static run() {
        console.log('Hello World !');
    }

    constructor(private _sample: string) { }

    get sample(): string {
        return this._sample;
    }
}

Main.run();
