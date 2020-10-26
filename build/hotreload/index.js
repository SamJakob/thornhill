const ON_DEATH = require('death');
const ChildProcess = require('child_process');
const Chokidar = require('chokidar');
const Consola = require('consola');
const Net = require('net');
const Ora = require('ora');

const config = require('../../thornhill-build-config.json');
const { Socket } = require('dgram');

const client = new Net.Socket();
const logger = Consola.create();



//
// File Watcher
//
const startWatcher = () => {
    const watcher = Chokidar.watch(config.watchDirs, {
        persistent: true,
        usePolling: true
    });

    watcher.on('change', () => {
        console.log("");

        let startTime = new Date().getTime();
        let recompiling = Ora("Recompiling...").start();

        compilerProcess = ChildProcess.spawn("make", [ "thornhill" ]);
        compilerProcess.on('exit', (exitCode, signal) => {
            let compileTime = Math.round((new Date().getTime() - startTime) / 1000);

            if (signal != null) {
                recompiling.fail(`The compiler was interrupted. (${compileTime}s)`);
                return;
            }

            if (exitCode == 0) {
                restartEmulator();
                recompiling.succeed(`Recompiled Thornhill. (${compileTime}s)`);
            } else
                recompiling.fail(`Failed to recompile Thornhill. (${compileTime}s)`);
        });

        
    });
    
    watcher.on('ready', () => {
        logger.info("Initial scan complete. Ready for changes.");
    });
};


//
// Socket
//

client.on('close', () => {
    console.log("Connection closed.");
});

client.on('error', (ex) => {
    if (ex.code == 'ECONNREFUSED') {
        logger.error(`Failed to ${ex.syscall} to virtual machine. Is QMP listening on ${ex.address}:${ex.port}?`);
        return;
    }

    logger.error(ex);
});


//
// QEMU Methods
//
let writeEmulatorCommand = (command) => {
    client.write(JSON.stringify(command));
}

let restartEmulator = () => {
    writeEmulatorCommand({ execute: "system_reset" });
}


//
// Application
//

let emulatorProcess;

(async () => {

    logger.info("Starting emulator...");
    emulatorProcess = ChildProcess.spawn("make", [ "emulator" ]);

    emulatorProcess.on('exit', () => {
        process.kill(process.pid, 'SIGINT');
    });

    let waitForEmulator = Ora("Waiting for emulator...").start();
    await new Promise((resolve) => {
        setTimeout(resolve, 1000);
    });
    waitForEmulator.succeed();

    logger.info("Starting file watcher...");
    startWatcher();

    logger.info("Connecting to virtual machine...");
    client.connect(4444, 'localhost', () => {
        logger.success("Connected to virtual machine...");

        writeEmulatorCommand({ execute: "qmp_capabilities" });
    });

})();

ON_DEATH((sig, err) => {

    if (emulatorProcess != null)
        emulatorProcess.kill('SIGTERM');
    
    console.log("");
    console.log("Goodbye!");
    process.exit(0);

});