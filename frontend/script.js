const document1 = document.getElementById("document1");
const document2 = document.getElementById("document2");

const file1Name = document.getElementById("file1Name");
const file2Name = document.getElementById("file2Name");

const checkButton = document.getElementById("checkButton");

document1.addEventListener("change", function () {

    if (document1.files.length > 0) {
        file1Name.textContent = document1.files[0].name;
    }

});

document2.addEventListener("change", function () {

    if (document2.files.length > 0) {
        file2Name.textContent = document2.files[0].name;
    }

});

checkButton.addEventListener("click", function () {

    if (document1.files.length === 0 ||
        document2.files.length === 0) {

        alert("Please select both documents.");

        return;
    }

    alert("Documents selected successfully!");

});