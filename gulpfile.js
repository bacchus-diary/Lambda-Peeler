var gulp = require('gulp'),
del = require('del'),
typings = require('gulp-typings'),
webpack = require('gulp-webpack'),
webpackConfig = require('./webpack.config.js');


gulp.task('watch', ['clean', 'typings'], function(){
    return webpack({ watch: true });
});

gulp.task('build', ['clean', 'typings'], function(){
    gulp.src(['./src/main/**/*.ts'])
    .pipe(webpack(webpackConfig))
    .pipe(gulp.dest('./build'));
});

gulp.task('clean', function(){
    return del('www/build');
});

gulp.task('clean-typings', function(){
    return del('typings');
});

gulp.task("typings", ['clean-typings'], function(){
    return gulp.src("./typings.json")
    .pipe(typings()); //will install all typingsfiles in pipeline.
});
