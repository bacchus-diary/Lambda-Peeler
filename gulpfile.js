var gulp = require('gulp'),
del = require('del'),
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
    return del(['*_bundle.js']);
});

gulp.task('clean-typings', () => {
    return del(['typings']);
});

gulp.task('test', ['build'], () => {
    return gulp.src('main_bundle.js').pipe(jasmine());
});
