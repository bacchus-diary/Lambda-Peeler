export const CV = require("opencv");

export function toPromise<T>(proc: (cb: (error, data: T) => void) => void): Promise<T> {
    return new Promise((resolve, reject) => {
        proc((error, data) => {
            if (error) reject(error);
            else resolve(data);
        });
    });
}
