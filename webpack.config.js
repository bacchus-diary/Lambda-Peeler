var path = require('path');

module.exports = {
  entry: [
    'babel-polyfill',
    'lodash',
    path.resolve('src/main/index')
  ],
  output: {
    path: path.resolve('build'),
    filename: 'app.bundle.js',
    pathinfo: false // show module paths in the bundle, handy for debugging
  },
  module: {
    loaders: [
      {
        test: /\.ts$/,
        loader: 'babel-loader?presets[]=es2015,presets[]=stage-0!ts-loader',
        include: path.resolve('src/main'),
        exclude: /node_modules/
      }
    ],
    noParse: [
      /aws-sdk/
    ]
  },
  resolve: {
    root: ['app'],
    alias: {
      'aws-sdk': path.resolve('node_modules/aws-sdk/dist/aws-sdk')
    },
    extensions: ["", ".js", ".ts"]
  }
};
