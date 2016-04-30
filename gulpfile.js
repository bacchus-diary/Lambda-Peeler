var gulp = require('gulp'),
cp = require('child_process'),
fs = require('fs'),
del = require('del'),
zip = require('gulp-zip'),
replace = require('gulp-replace'),
typings = require('gulp-typings'),
webpack = require('gulp-webpack');

gulp.task('build', ['typings', 'inject-tests'], () => {
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

gulp.task('inject-tests', () => {
    const dir = './src/spec';
    function specs() {
        return fs.readdirSync(dir)
        .filter((x) => x.endsWith('_spec.ts'))
        .map((x) => `spec('./${x}')`)
        .join(',\n');
    }
    return gulp.src(`${dir}/_specs.ts`)
    .pipe(replace('/* __FATHENS_ALL_SPECS__ */', specs()))
    .pipe(gulp.dest(dir));
});

gulp.task('test', ['build'], () => {
    return new Promise((resolve, reject) => {
        const main_bundle = require('./main_bundle');
        main_bundle.handler('TEST', null, (error, result) => {
            if (error) {
                reject(error);
            } else {
                resolve(result);
            }
        });
    });
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
