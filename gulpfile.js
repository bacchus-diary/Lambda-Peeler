var gulp = require('gulp'),
fs = require('fs'),
del = require('del'),
zip = require('gulp-zip'),
babel = require('gulp-babel'),
ts = require('gulp-typescript'),
replace = require('gulp-replace'),
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

gulp.task('build-test', ['build'], () => {
    const proj = ts.createProject('tsconfig.json');
    return gulp.src('spec/**/*.ts')
    .pipe(replace(/(import\s+.+\s+from\s+')\.\..*\/src\/.+(';\s*)/g, '$1../main_bundle$2'))
    .pipe(ts(proj))
    .pipe(babel({
        presets: ['es2015', 'stage-0']
    })).pipe(gulp.dest('spec/'));
});

gulp.task('test', ['build-test'], () => {
    return gulp.src('spec/**/*.js').pipe(jasmine());
});

gulp.task('pack', ['test'], () => {
    const exclude = /aws\-sdk/;
    const externals = require('./webpack.config.js').externals;
    const sources = ['./*_bundle.js', './spec/**', '!./spec/**/*.ts'];
    fs.readdirSync('./node_modules')
    .filter((x) => !exclude.test(x))
    .filter((x) => externals.some((y) => y.test(x)))
    .forEach((x) => sources.push(`./node_modules/${x}/**`));
    console.log(`Packing: ${JSON.stringify(sources, null, 4)}`);
    return gulp.src(sources, { base: './' })
    .pipe(zip('main.zip'))
    .pipe(gulp.dest('./'));
});
