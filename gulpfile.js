var gulp = require('gulp'),
_ = require('lodash'),
path = require('path'),
fs = require('fs'),
del = require('del'),
zip = require('gulp-zip'),
shell = require('gulp-shell'),
rename = require('gulp-rename'),
runSeq = require('run-sequence'),
typings = require('gulp-typings'),
webpack = require('webpack-stream');

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

gulp.task('haskell', shell.task([
    "stack --install-ghc --allow-different-user install",
    "haskell/install.sh"
]));

gulp.task('only_c', shell.task([
    "g++ -L/var/task/lib -I/var/task/include haskell/Peeler/src/main/OpenCV/Video.cpp -lstdc++ -llevmar -lopencv_aruco -lopencv_bgsegm -lopencv_bioinspired -lopencv_calib3d -lopencv_ccalib -lopencv_core -lopencv_datasets -lopencv_dnn -lopencv_dpm -lopencv_face -lopencv_features2d -lopencv_flann -lopencv_fuzzy -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_line_descriptor -lopencv_ml -lopencv_objdetect -lopencv_optflow -lopencv_photo -lopencv_plot -lopencv_reg -lopencv_rgbd -lopencv_saliency -lopencv_shape -lopencv_stereo -lopencv_stitching -lopencv_structured_light -lopencv_superres -lopencv_surface_matching -lopencv_text -lopencv_tracking -lopencv_videoio -lopencv_video -lopencv_videostab -lopencv_xfeatures2d -lopencv_ximgproc -lopencv_xobjdetect -lopencv_xphoto -o haskell/Peeler/only_c"
]));

gulp.task('test-only', [], (cb) => {
    require('./main_bundle').handler('TEST', null, cb);
});

gulp.task('test', ['only_c', 'build', 'haskell'], (cb) => {
    runSeq('test-only', cb);
});

gulp.task('pack', ['test'], () => {
    const exclude = /aws\-sdk/;
    const externals = require('./webpack.config.js').externals;

    const sources = ['./*_bundle.js', './haskell/peeler', '**/resources/**', './haskell/Peeler/only_c'];
    fs.readdirSync('./node_modules')
    .filter((x) => !exclude.test(x))
    .filter((x) => externals.some((y) => y.test(x)))
    .forEach((x) => sources.push(`./node_modules/${x}/**`));
    console.log(`Packing: ${JSON.stringify(sources, null, 4)}`);

    return gulp.src(sources, { base: './' })
    .pipe(zip('main.zip'))
    .pipe(gulp.dest('./'));
});

gulp.task('default', ['pack'], shell.task([
    "DIR=$(pwd) && cd /var/task && zip -ryD $DIR/main.zip lib"
]));
