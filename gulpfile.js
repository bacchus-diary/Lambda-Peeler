var gulp = require('gulp'),
cp = require('child_process'),
path = require('path'),
fs = require('fs'),
del = require('del'),
zip = require('gulp-zip'),
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

gulp.task('inject-tests', (cb) => {
    const dir = './src/spec';
    const target = `${dir}/_specs.json`;
    const filter = /.*_spec\.ts$/;
    const specs = [];
    function filesIn(parent) {
        fs.readdirSync(parent).forEach((name) => {
            const file = path.join(parent, name);
            if (fs.statSync(file).isDirectory()) {
                filesIn(file);
            } else if (filter.test(name)) {
                specs.push('./' + path.relative(dir, file));
            }
        });
    }
    filesIn(dir);
    fs.writeFile(target, JSON.stringify(specs), cb);
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
    const sources = ['./*_bundle.js'];
    fs.readdirSync('./node_modules')
    .filter((x) => !exclude.test(x))
    .filter((x) => externals.some((y) => y.test(x)))
    .forEach((x) => sources.push(`./node_modules/${x}/**`));
    console.log(`Packing: ${JSON.stringify(sources, null, 4)}`);
    return gulp.src(sources, { base: './' })
    .pipe(zip('main.zip'))
    .pipe(gulp.dest('./'));
});
