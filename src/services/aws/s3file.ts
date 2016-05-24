import {Logger} from "../../util/logging";
import * as AWS from "./aws";
const fs = require("fs");

const logger = new Logger("S3File");

export class S3File {
    constructor(private bucketName: string) {
        logger.debug(() => `Creting S3: ${AWS.S3}`);
        this.client = new AWS.S3();
    }

    private client: AWS.S3;

    private async invoke<R>(proc: (s3client) => AWS.Request): Promise<R> {
        return AWS.requestToPromise<R>(proc(this.client));
    }

    async read(path: string): Promise<string> {
        logger.debug(() => `Reading file: ${this.bucketName}:${path}`);
        const res = await this.invoke<{ Body: number[] }>((s3) => s3.getObject({
            Bucket: this.bucketName,
            Key: path
        }));
        return String.fromCharCode.apply(null, res.Body);
    }

    async download(path: string, dst: string): Promise<void> {
        logger.debug(() => `Reading file: ${this.bucketName}:${path}`);
        const res = await this.invoke<{ Body: number[] }>((s3) => s3.getObject({
            Bucket: this.bucketName,
            Key: path
        }));
        await new Promise((resolve, reject) => {
          fs.createWriteStream(dst).write(res.Body, (error) => {
            if (_.isNil(error)) {
              resolve();
            } else {
              reject(error);
            }
          });
        });
    }

    async upload(path: string, blob: Blob): Promise<void> {
        logger.debug(() => `Uploading file: ${this.bucketName}:${path}`);
        await this.invoke((s3) => s3.putObject({
            Bucket: this.bucketName,
            Key: path,
            Body: blob,
            ContentType: blob.type
        }));
    }

    async remove(path: string): Promise<void> {
        logger.debug(() => `Removing file: ${this.bucketName}:${path}`);
        await this.invoke((s3) => s3.deleteObject({
            Bucket: this.bucketName,
            Key: path
        }));
    }

    async copy(src: string, dst: string): Promise<void> {
        logger.debug(() => `Copying file: ${this.bucketName}:${src}=>${dst}`);
        await this.invoke((s3) => s3.copyObject({
            Bucket: this.bucketName,
            CopySource: `${this.bucketName}/${src}`,
            Key: dst
        }));
    }

    async move(src: string, dst: string): Promise<void> {
        await this.copy(src, dst);
        await this.remove(src);
    }

    async list(path: string): Promise<Array<string>> {
        logger.debug(() => `Listing up: ${path}`);
        const res = await this.invoke<{ Contents: { Key: string }[] }>((s3) => s3.listObjects({
            Bucket: this.bucketName,
            Prefix: path
        }));
        return res.Contents.map((x) => x.Key);
    }

    async exists(path: string): Promise<boolean> {
        logger.debug(() => `Checking exists: ${this.bucketName}:${path}`);
        try {
            const res = await this.invoke<{ ContentLength: number }>((s3) => s3.headObject({
                Bucket: this.bucketName,
                Key: path
            }));
            return res != null;
        } catch (ex) {
            logger.warn(() => `Error on checking exists: ${this.bucketName}:${path}`);
            return false;
        }
    }

    async url(path: string, expiresInSeconds: number): Promise<string> {
        const s3: any = await this.client;
        logger.debug(() => `Getting url of file: ${this.bucketName}:${path}`);
        try {
            return s3.getSignedUrl("getObject", {
                Bucket: this.bucketName,
                Key: path,
                Expires: expiresInSeconds
            });
        } catch (ex) {
            logger.warn(() => `Error on getting url: ${ex}`);
        }
    }
}
