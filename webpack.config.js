var path = require('path'),
fs = require('fs'),
webpack = require('webpack');

var entries = {
    main:[
        'babel-polyfill',
        path.resolve('src/index')
    ],
    spec: ['babel-polyfill']
};
fs.readdirSync('src/spec')
.filter((x) => x.endsWith('Spec.ts'))
.map((x) => 'src/spec/' + x.substring(0, x.length - 3))
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
        /aws\-sdk/
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
        root: ['app'],
        extensions: ["", ".js", ".ts"]
    }
};
