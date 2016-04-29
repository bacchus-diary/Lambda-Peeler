const path = require('path'),
fs = require('fs'),
webpack = require('webpack');

const entries = {
    main:[
        'babel-polyfill',
        path.resolve('src/index')
    ],
    spec: ['babel-polyfill']
};
const specDir = 'src/spec';
fs.readdirSync(specDir)
.filter((x) => x.endsWith('_spec.ts'))
.map((x) => `${specDir}/${x.substring(0, x.length - 3)}`)
.forEach((x) => entries.spec.push(path.resolve(x)));

module.exports = {
    entry: entries,
    output: {
        library: "[name]",
        libraryTarget: "commonjs2",
        filename: '[name]_bundle.js',
        pathinfo: false // show module paths in the bundle, handy for debugging
    },
    externals: [
        /^jasmine$/,
        /^opencv$/,
        /^aws\-sdk$/
    ],
    target: 'node',
    plugins: [
        new webpack.ProvidePlugin({
            _: "lodash"
        })
    ],
    module: {
        loaders: [
            {
                test: /\.ts$/,
                loader: 'babel-loader?presets[]=es2015,presets[]=stage-0!ts-loader',
                include: [path.resolve('src')],
                exclude: /node_modules/
            },
            {
                test: '\.json$',
                loader: 'json'
            }
        ]
    },
    resolve: {
        extensions: ["", ".js", ".ts"]
    }
};
