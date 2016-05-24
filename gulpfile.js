var gulp = require('gulp'),
_ = require('lodash'),
path = require('path'),
fs = require('fs'),
del = require('del'),
typings = require('gulp-typings'),
webpack = require('gulp-webpack');

function filesRecursive(parent, regex, callback) {
    fs.readdir(parent, (error, names) => {
        if (error) {
            callback(error);
        } else {
            Promise.all(
                names.map((name) => new Promise((resolve, reject) => {
                    const file = path.join(parent, name);
                    fs.stat(file, (error, stats) => {
                        if (error) {
                            reject(error);
                        } else if (stats.isDirectory()) {
                            filesRecursive(file, regex,
                                (error, files) => error ? reject(error) : resolve(files)
                            );
                        } else {
                            resolve(regex.test(name) ? file : null);
                        }
                    });
                }))
            ).then((lists) => callback(null, _.flatten(_.compact(lists))))
            .catch((error) => callback(error));
        }
    });
}

gulp.task('build', ['typings', 'inject-tests'], () => {
    webpackConfig = require('./webpack.config.js');
    webpackConfig.watch = process.argv.indexOf('--watch') > -1;

    return webpack(webpackConfig).pipe(gulp.dest('./'));
});

gulp.task('typings', ['clean-typings'], () => {
    return gulp.src('./typings.json').pipe(typings());
});

gulp.task('clean', () => del(['*_bundle.js', '*.zip']));

gulp.task('clean-typings', () => del(['typings']));

gulp.task('inject-tests', (cb) => {
    const dir = './src/spec';
    const target = `${dir}/_specs.json`;
    filesRecursive(dir, /.*_spec\.ts$/, (error, files) => {
        if (error) {
            cb(error);
        } else {
            const specs = files.map((file) => './' + path.relative(dir, file));
            fs.writeFile(target, JSON.stringify(specs), cb);
        }
    });
});

gulp.task('test', ['build'], (cb) => {
    require('./main_bundle').handler('TEST', null, cb);
});

gulp.task('default', ['test'], (cb) => {
    const args = ['-r', 'main.zip', './main_bundle.js', './lib/', './share/'];

    const exclude = /aws\-sdk/;
    const externals = require('./webpack.config.js').externals;
    fs.readdirSync('./node_modules')
    .filter((x) => !exclude.test(x))
    .filter((x) => externals.some((y) => y.test(x)))
    .forEach((x) => args.push(`./node_modules/${x}/`));

    const ps = require('child_process').spawn('zip', args, { stdio: 'inherit' });
    ps.on('close', (code) => {
      cb(code);
    });
});
