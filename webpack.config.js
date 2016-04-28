var path = require('path')
webpack = require('webpack');

module.exports = {
    entry: [
        'babel-polyfill',
        path.resolve('src/index')
    ],
    output: {
        library: "[name]",
        libraryTarget: "commonjs2",
        filename: '[name].js',
        pathinfo: false // show module paths in the bundle, handy for debugging
    },
    externals: [
        /systemjs/,
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
