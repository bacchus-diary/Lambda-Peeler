var gulp = require('gulp'),
fs = require('fs'),
del = require('del'),
zip = require('gulp-zip'),
ts = require('gulp-typescript'),
jasmine = require('gulp-jasmine'),
typings = require('gulp-typings'),
webpack = require('gulp-webpack');

gulp.task('build', ['typings'], () => {
    webpackConfig = require('./webpack.config.js');
    webpackConfig.watch = process.argv.indexOf('--watch') > -1;

    return webpack(webpackConfig).pipe(gulp.dest('./'));
});

gulp.task('typings', ['clean-typings'], () => {
    return gulp.src('./typings.json').pipe(typings());
});

gulp.task('clean', () => {
    return del(['*_bundle.js', '*.zip']);
});

gulp.task('clean-typings', () => {
    return del(['typings']);
});

gulp.task('test', ['build'], () => {
    return gulp.src('spec_bundle.js').pipe(jasmine());
});

gulp.task('pack', ['test'], () => {
    const exclude = /aws\-sdk/;
    const externals = require('./webpack.config.js').externals;
    const sources = fs.readdirSync('./node_modules')
    .filter((x) => !exclude.test(x))
    .filter((x) => externals.some((y) => y.test(x)))
    .map((x) => `./node_modules/${x}/**`);
    sources.push('./*_bundle.js');
    console.log(`Packing: ${JSON.stringify(sources, null, 4)}`);
    return gulp.src(sources, { base: './' })
    .pipe(zip('main.zip'))
    .pipe(gulp.dest('./'));
});
