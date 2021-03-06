/* global process require */
const {Builder, By, until} = require('selenium-webdriver');
const firefox = require('selenium-webdriver/firefox');
const assert = require('assert').strict;


(async function example() {
    var options = new firefox.Options();
    options.addArguments("-headless");
    let driver = new Builder()
        .forBrowser('firefox')
        .setFirefoxOptions(options)
        .setFirefoxService(
            new firefox.ServiceBuilder(
                `./node_modules/geckodriver/geckodriver${process.platform === 'win32' ? '.exe' : ''}`))
        .build();
    try {
        await driver.get(`file:///${process.cwd()}/SETUP/tests/qunit.html`);
        await driver.wait(until.elementLocated(By.className('passed')), 10000);
        var passed = parseInt(await (await driver.findElement(By.className('passed'))).getText(), 10);
        var total = parseInt(await (await driver.findElement(By.className('total'))).getText(), 10);
        var failed = parseInt(await (await driver.findElement(By.className('failed'))).getText(), 10);
        console.log(`${passed} assertions of ${total} passed, ${failed} failed.`);
        assert(passed === total && failed === 0, 'Some test failed.');
    } finally {
        await driver.quit();
    }
})();