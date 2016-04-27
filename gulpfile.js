var gulp = require('gulp'),
del = require('del'),
typings = require('gulp-typings'),
webpack = require('gulp-webpack');

webpackConfig = require('./webpack.config.js');
webpackConfig.watch = process.argv.indexOf('--watch') > -1;

gulp.task('build', ['clean', 'typings'], function(){
    return gulp.src(['./src/main/**/*.ts'])
    .pipe(webpack(webpackConfig))
    .pipe(gulp.dest('./build'));
});

gulp.task('clean', function(){
    return del('build');
});

gulp.task('clean-typings', function(){
    return del('typings');
});

gulp.task('typings', ['clean-typings'], function(){
    return gulp.src('./typings.json').pipe(typings());
});
