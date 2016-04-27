var gulp = require('gulp'),
del = require('del'),
ts = require('gulp-typescript'),
jasmine = require('gulp-jasmine'),
typings = require('gulp-typings'),
webpack = require('gulp-webpack');

gulp.task('build', ['clean', 'typings'], () => {
    webpackConfig = require('./webpack.config.js');
    webpackConfig.watch = process.argv.indexOf('--watch') > -1;

    return gulp.src(['./src/**/*.ts'])
    .pipe(webpack(webpackConfig))
    .pipe(gulp.dest('./build'));
});

gulp.task('clean', () => {
    return del('build');
});

gulp.task('clean-typings', () => {
    return del('typings');
});

gulp.task('typings', ['clean-typings'], () => {
    return gulp.src('./typings.json').pipe(typings());
});

gulp.task('build-test', () => {
    webpackConfig = require('./spec/support/webpack.config.js');
    return gulp.src('./spec/**/*.ts')
    .pipe(webpack(webpackConfig))
    .pipe(gulp.dest('./spec'));
});

gulp.task('test', ['build-test'], () => {
    return gulp.src('spec/**/*.js')
    .pipe(jasmine());
});
