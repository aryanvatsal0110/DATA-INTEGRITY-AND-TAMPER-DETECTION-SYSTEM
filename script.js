
// ==========================================
// TEMPERD - COMPLETE FRONTEND JAVASCRIPT
// ==========================================

const API_BASE_URL = "http://localhost:8080";

// ==========================================
// 1. APPLICATION STATE
// ==========================================

let registerFile = null;
let verifyFile = null;

let registeredFileId =
    localStorage.getItem("temperd_file_id") || "";

let latestReportUrl =
    localStorage.getItem("temperd_report_url") || "";

let latestReportId =
    localStorage.getItem("temperd_report_id") || "";


// ==========================================
// 2. PAGE NAVIGATION
// ==========================================

const navLinks = document.querySelectorAll(".nav-link");
const pages = document.querySelectorAll(".page");

const pageNames = {
    dashboard: "Dashboard",
    register: "Register File",
    verify: "Verify File",
    analysis: "Tamper Analysis",
    reports: "Reports"
};

function showPage(pageId) {

    pages.forEach(page => {
        page.classList.remove("active");
    });

    navLinks.forEach(link => {
        link.classList.remove("active");
    });

    const selectedPage = document.getElementById(pageId);

    if (!selectedPage) return;

    selectedPage.classList.add("active");

    const activeLink = document.querySelector(
        `.nav-link[data-page="${pageId}"]`
    );

    if (activeLink) {
        activeLink.classList.add("active");
    }

    document.getElementById("page-heading").textContent =
        pageNames[pageId] || pageId;

    document.getElementById("breadcrumb").textContent =
        pageNames[pageId] || pageId;

    if (pageId === "reports") {
        displayLatestReport();
    }
}

navLinks.forEach(link => {

    link.addEventListener("click", () => {
        showPage(link.dataset.page);
    });

});

document.querySelectorAll("[data-go]").forEach(button => {

    button.addEventListener("click", () => {
        showPage(button.dataset.go);
    });

});


// ==========================================
// 3. FILE INFORMATION
// ==========================================

function getFileType(fileName) {

    const extension = fileName.split(".").pop().toLowerCase();

    const types = {
        txt: "Text File",
        csv: "CSV File",
        pdf: "PDF Document",
        docx: "Word Document",
        doc: "Word Document"
    };

    return types[extension] || "Other File";
}

function formatFileSize(bytes) {

    if (bytes < 1024) {
        return bytes + " Bytes";
    }

    if (bytes < 1024 * 1024) {
        return (bytes / 1024).toFixed(2) + " KB";
    }

    return (bytes / (1024 * 1024)).toFixed(2) + " MB";
}


// ==========================================
// 4. DYNAMIC RESULT PANELS
// ==========================================

function createPanel(id, buttonId) {

    let panel = document.getElementById(id);

    if (panel) return panel;

    panel = document.createElement("div");

    panel.id = id;

    panel.style.cssText = `
        margin-top: 24px;
        padding: 22px;
        border-radius: 12px;
        background: #111c30;
        border: 1px solid #263b5b;
        color: #e5edf8;
        overflow-wrap: anywhere;
    `;

    const button = document.getElementById(buttonId);

    button.insertAdjacentElement("afterend", panel);

    return panel;
}

function showMessage(panel, message, type = "info") {

    const colors = {
        success: "#22c58b",
        error: "#ef6464",
        warning: "#f0b44d",
        info: "#65a9ff"
    };

    panel.replaceChildren();

    const heading = document.createElement("h3");

    heading.textContent = message;

    heading.style.color = colors[type] || colors.info;

    panel.appendChild(heading);
}

function addDetail(panel, label, value) {

    const row = document.createElement("p");

    const strong = document.createElement("strong");

    strong.textContent = label + ": ";

    const span = document.createElement("span");

    span.textContent =
        value === undefined || value === null
            ? "N/A"
            : String(value);

    row.append(strong, span);

    panel.appendChild(row);
}

function addSection(panel, headingText) {

    const heading = document.createElement("h3");

    heading.textContent = headingText;

    heading.style.marginTop = "20px";

    panel.appendChild(heading);

    return heading;
}


// ==========================================
// 5. FILE PREVIEW
// ==========================================

function displayFile(file, previewId, buttonId) {

    const preview = document.getElementById(previewId);
    const button = document.getElementById(buttonId);

    if (!file) return;

    preview.innerHTML = `
        <div>
            <strong></strong>
            <p class="file-details"></p>
        </div>
        <span style="color:#159b70;font-weight:600">
            Selected
        </span>
    `;

    preview.querySelector("strong").textContent = file.name;

    preview.querySelector(".file-details").textContent =
        getFileType(file.name) + " · " + formatFileSize(file.size);

    preview.classList.remove("hidden");

    button.disabled = false;
}


// ==========================================
// 6. REGISTER FILE SELECTION
// ==========================================

const registerInput = document.getElementById("register-input");

registerInput.addEventListener("change", () => {

    registerFile = registerInput.files[0];

    if (registerFile) {

        displayFile(
            registerFile,
            "register-preview",
            "register-button"
        );

    }

});


// ==========================================
// 7. VERIFY FILE SELECTION
// ==========================================

const verifyInput = document.getElementById("verify-input");

verifyInput.addEventListener("change", () => {

    verifyFile = verifyInput.files[0];

    if (verifyFile) {

        displayFile(
            verifyFile,
            "verify-preview",
            "verify-button"
        );

    }

});


// ==========================================
// 8. DRAG AND DROP
// ==========================================

function setupDragDrop(dropId, inputId, callback) {

    const dropArea = document.getElementById(dropId);
    const input = document.getElementById(inputId);

    ["dragenter", "dragover"].forEach(eventName => {

        dropArea.addEventListener(eventName, event => {

            event.preventDefault();

            dropArea.classList.add("dragover");

        });

    });

    ["dragleave", "drop"].forEach(eventName => {

        dropArea.addEventListener(eventName, event => {

            event.preventDefault();

            dropArea.classList.remove("dragover");

        });

    });

    dropArea.addEventListener("drop", event => {

        const file = event.dataTransfer.files[0];

        if (file) {
            callback(file);
        }

    });

    input.addEventListener("click", event => {
        event.stopPropagation();
    });

}

setupDragDrop("register-drop", "register-input", file => {

    registerFile = file;

    displayFile(
        file,
        "register-preview",
        "register-button"
    );

});

setupDragDrop("verify-drop", "verify-input", file => {

    verifyFile = file;

    displayFile(
        file,
        "verify-preview",
        "verify-button"
    );

});


// ==========================================
// 9. CLEAR REGISTER FILE
// ==========================================

document.getElementById("register-clear")
    .addEventListener("click", () => {

        registerFile = null;

        registerInput.value = "";

        document.getElementById("register-preview")
            .classList.add("hidden");

        document.getElementById("register-button")
            .disabled = true;

    });


// ==========================================
// 10. REGISTER FILE WITH C++ BACKEND
// ==========================================

document.getElementById("register-button")
    .addEventListener("click", async () => {

        if (!registerFile) {
            alert("Please select a file first!");
            return;
        }

        const button = document.getElementById("register-button");

        const panel = createPanel(
            "register-result",
            "register-button"
        );

        button.disabled = true;
        button.textContent = "Registering...";

        try {

            const formData = new FormData();

            formData.append("file", registerFile);

            const response = await fetch(
                `${API_BASE_URL}/api/register`,
                {
                    method: "POST",
                    body: formData
                }
            );

            const result = await response.json();

            if (!response.ok || !result.success) {
                throw new Error(
                    result.message || "File registration failed."
                );
            }

            if (!result.fileId) {
                throw new Error(
                    "Registration response did not contain a File ID."
                );
            }

            // Save registered file information.
            registeredFileId = result.fileId;
            localStorage.setItem(
                "temperd_original_filename",
                result.fileName
            );

            localStorage.setItem(
                "temperd_file_id",
                registeredFileId
            );

            showMessage(
                panel,
                "File registered successfully!",
                "success"
            );

            addDetail(panel, "File Name", result.fileName);
            addDetail(panel, "File ID", result.fileId);
            addDetail(panel, "File Size", formatFileSize(result.fileSize));
            addDetail(panel, "Block Size", formatFileSize(result.blockSize));
            addDetail(panel, "Total Blocks", result.totalBlocks);
            addDetail(panel, "Merkle Root", result.merkleRoot);

            addSection(panel, "Next Step");

            const instruction = document.createElement("p");

            instruction.textContent =
                "Your original file baseline has been saved. " +
                "You can now open Verify File and upload a file " +
                "to check its integrity.";

            panel.appendChild(instruction);

            console.log("Registration response:", result);

        } catch (error) {

            console.error("Registration error:", error);

            showMessage(
                panel,
                "Registration failed: " + error.message,
                "error"
            );

        } finally {

            button.disabled = false;
            button.textContent = "Register File";

        }

    });


// ==========================================
// 11. VERIFY FILE WITH C++ BACKEND
// ==========================================

document.getElementById("verify-button")
    .addEventListener("click", async () => {

        if (!verifyFile) {
            alert("Please select a file first!");
            return;
        }
        // Check whether the selected file matches
// the registered original filename.

const originalFileName =
    localStorage.getItem("temperd_original_filename");

if (
    originalFileName &&
    verifyFile.name !== originalFileName
) {

    const proceed = confirm(
        "WARNING: The selected file has a different filename " +
        "from the registered original.\n\n" +

        "Registered file: " + originalFileName + "\n" +
        "Selected file: " + verifyFile.name + "\n\n" +

        "This might be a different file.\n" +
        "Do you still want to continue verification?"
    );

    if (!proceed) {
        return;
    }
}

        if (!registeredFileId) {

            alert(
                "No registered File ID found. " +
                "Please register an original file first."
            );

            return;
        }

        const button = document.getElementById("verify-button");

        const panel = createPanel(
            "verify-result",
            "verify-button"
        );

        button.disabled = true;
        button.textContent = "Verifying...";

        try {

            const formData = new FormData();

            formData.append("file", verifyFile);

            formData.append("fileId", registeredFileId);

            const response = await fetch(
                `${API_BASE_URL}/api/verify`,
                {
                    method: "POST",
                    body: formData
                }
            );

            const result = await response.json();

            if (!response.ok || !result.success) {
                throw new Error(
                    result.message || "File verification failed."
                );
            }

            showMessage(
                panel,
                result.integrityMatch
                    ? "FILE INTEGRITY VERIFIED"
                    : "TAMPERING DETECTED",
                result.integrityMatch ? "success" : "error"
            );

            addDetail(panel, "File ID", result.fileId);
            addDetail(panel, "Original Blocks", result.originalBlockCount);
            addDetail(panel, "Current Blocks", result.currentBlockCount);

            addDetail(
                panel,
                "Integrity Status",
                result.integrityMatch
                    ? "No modifications detected"
                    : "File has been modified"
            );

            addDetail(panel, "Original Merkle Root", result.originalRoot);
            addDetail(panel, "Current Merkle Root", result.currentRoot);
            

            
// ------------------------------------------
// DISPLAY TEXT-LEVEL CHANGES
// ------------------------------------------

addSection(panel, "Detected Content Changes");

if (result.textChanges?.length) {

    result.textChanges.forEach(change => {

        // Support both camelCase and snake_case JSON fields.
        const changeType =
            change.changeType ?? change.change_type ?? "MODIFIED";

        const originalLine =
            change.originalLineNumber ??
            change.original_line ??
            change.originalLine ??
            "N/A";

        const currentLine =
            change.currentLineNumber ??
            change.current_line ??
            change.currentLine ??
            "N/A";

        const originalText =
            change.originalText ??
            change.original_text ??
            "";

        const currentText =
            change.currentText ??
            change.current_text ??
            "";

        const removedWords =
            change.removedWords ??
            change.removed_words ??
            [];

        const addedWords =
            change.addedWords ??
            change.added_words ??
            [];

        const card = document.createElement("div");

        card.style.cssText = `
            margin-top: 14px;
            padding: 18px;
            background: #192943;
            border: 1px solid #304969;
            border-radius: 10px;
        `;

        // Change heading
        const heading = document.createElement("div");

        heading.style.cssText = `
            display: flex;
            justify-content: space-between;
            flex-wrap: wrap;
            gap: 10px;
            margin-bottom: 16px;
        `;

        const changeLabel = document.createElement("strong");

        changeLabel.textContent = changeType;
        changeLabel.style.color = "#80b9ff";

        const lineLabel = document.createElement("span");

        lineLabel.textContent =
            "Original line: " + originalLine +
            " | Current line: " + currentLine;

        lineLabel.style.cssText = `
            color: #a9bad0;
            font-size: 12px;
        `;

        heading.append(changeLabel, lineLabel);
        card.appendChild(heading);

        // Original content
        const originalLabel = document.createElement("p");

        originalLabel.textContent = "ORIGINAL CONTENT";
        originalLabel.style.cssText = `
            color: #ff9292;
            font-size: 11px;
            font-weight: 700;
            margin-bottom: 7px;
        `;

        const originalContent = document.createElement("div");

        originalContent.textContent = originalText || "(Empty)";
        originalContent.style.cssText = `
            background: #ef646420;
            color: #ffaaaa;
            padding: 12px;
            border-radius: 7px;
            margin-bottom: 14px;
            white-space: pre-wrap;
            overflow-wrap: anywhere;
        `;

        // Modified content
        const currentLabel = document.createElement("p");

        currentLabel.textContent = "MODIFIED CONTENT";
        currentLabel.style.cssText = `
            color: #62e2ad;
            font-size: 11px;
            font-weight: 700;
            margin-bottom: 7px;
        `;

        const currentContent = document.createElement("div");

        currentContent.textContent = currentText || "(Empty)";
        currentContent.style.cssText = `
            background: #22c58b20;
            color: #80e8bd;
            padding: 12px;
            border-radius: 7px;
            white-space: pre-wrap;
            overflow-wrap: anywhere;
        `;

        card.append(
            originalLabel,
            originalContent,
            currentLabel,
            currentContent
        );

        // Removed and added words
        if (removedWords.length || addedWords.length) {

            const wordChanges = document.createElement("div");

            wordChanges.style.cssText = `
                display: flex;
                flex-wrap: wrap;
                gap: 20px;
                margin-top: 16px;
                font-size: 12px;
            `;

            if (removedWords.length) {

                const removed = document.createElement("p");

                removed.textContent =
                    "− Removed: " + removedWords.join(", ");

                removed.style.color = "#ff9292";

                wordChanges.appendChild(removed);
            }

            if (addedWords.length) {

                const added = document.createElement("p");

                added.textContent =
                    "+ Added: " + addedWords.join(", ");

                added.style.color = "#62e2ad";

                wordChanges.appendChild(added);
            }

            card.appendChild(wordChanges);
        }

        panel.appendChild(card);

    });

} else {

    const message = document.createElement("p");

    message.textContent =
        "No text-level changes available for this file.";

    message.style.cssText = `
        color: #91a3bb;
        margin-top: 12px;
    `;

    panel.appendChild(message);
}

            /* addDetail(
                panel,
                "Changed Blocks",
                result.changedBlocks?.length ?? 0
            );

            addDetail(
                panel,
                "Text Changes",
                result.textChanges?.length ?? 0
            );

            // Display changed block information.
            if (result.changedBlocks?.length) {

                addSection(panel, "Changed Blocks");

                result.changedBlocks.forEach(block => {

                    const item = document.createElement("p");

                    item.textContent =
                        typeof block === "object"
                            ? JSON.stringify(block)
                            : "Block " + block;

                    panel.appendChild(item);

                });

            }

            // Display text-level differences.
            if (result.textChanges?.length) {

                addSection(panel, "Text-Level Changes");

                result.textChanges.forEach(change => {

                    const card = document.createElement("div");

                    card.style.cssText = `
                        margin-top: 12px;
                        padding: 14px;
                        background: #192943;
                        border-radius: 8px;
                    `;

                    const type = document.createElement("strong");

                    type.textContent = change.change_type || "CHANGE";

                    card.appendChild(type);

                    addDetail(card, "Original Line", change.original_line);
                    addDetail(card, "Current Line", change.current_line);
                    addDetail(card, "Original Text", change.original_text);
                    addDetail(card, "Current Text", change.current_text);

                    addDetail(
                        card,
                        "Removed Words",
                        change.removed_words?.join(", ")
                    );

                    addDetail(
                        card,
                        "Added Words",
                        change.added_words?.join(", ")
                    );

                    panel.appendChild(card);

                });

            }
 */
            // Save report details.
            if (result.reportUrl) {

                latestReportUrl = result.reportUrl;

                localStorage.setItem(
                    "temperd_report_url",
                    latestReportUrl
                );

            }

            if (result.reportId) {

                latestReportId = result.reportId;

                localStorage.setItem(
                    "temperd_report_id",
                    latestReportId
                );

            }

            addSection(panel, "Verification Report");

            addDetail(panel, "Report ID", result.reportId);

            const reportButton = document.createElement("button");

            reportButton.textContent = "View Full Report";

            reportButton.style.cssText = `
                margin-top: 12px;
                padding: 11px 20px;
                border: none;
                border-radius: 7px;
                background: #2878d0;
                color: white;
                cursor: pointer;
                font-weight: 600;
            `;

            reportButton.addEventListener("click", () => {
                showPage("reports");
            });

            panel.appendChild(reportButton);

            console.log("Verification response:", result);

            // Make analysis page accessible.
            showPage("verify");

        } catch (error) {

            console.error("Verification error:", error);

            showMessage(
                panel,
                "Verification failed: " + error.message,
                "error"
            );

        } finally {

            button.disabled = false;
            button.textContent = "Verify File";

        }

    });


// ==========================================
// 12. REPORT RETRIEVAL
// ==========================================
/* 
async function displayLatestReport() {

    const reportsPage = document.getElementById("reports");

    let panel = document.getElementById("reports-result");

    if (!panel) {

        panel = document.createElement("div");

        panel.id = "reports-result";

        panel.style.cssText = `
            margin-top: 24px;
            padding: 22px;
            border-radius: 12px;
            background: #111c30;
            border: 1px solid #263b5b;
            color: #e5edf8;
            overflow-wrap: anywhere;
        `;

        reportsPage.appendChild(panel);

    }

    panel.replaceChildren();

    if (!latestReportUrl) {

        showMessage(
            panel,
            "No verification report available yet.",
            "info"
        );

        return;
    }

    showMessage(
        panel,
        "Loading verification report...",
        "info"
    );

    try {

        const response = await fetch(
            `${API_BASE_URL}${latestReportUrl}`
        );

        if (!response.ok) {
            throw new Error("Unable to retrieve report.");
        }

        const report = await response.json();

        panel.replaceChildren();

        showMessage(
            panel,
            "Verification Report Retrieved",
            "success"
        );

        addDetail(panel, "Report ID", latestReportId);

        addSection(panel, "Full Report Data");

        const pre = document.createElement("pre");

        pre.textContent = JSON.stringify(report, null, 2);

        pre.style.cssText = `
            white-space: pre-wrap;
            overflow-x: auto;
            padding: 16px;
            background: #0b1424;
            border-radius: 8px;
            font-size: 13px;
            line-height: 1.6;
        `;

        panel.appendChild(pre);

        const downloadButton = document.createElement("button");

        downloadButton.textContent = "Download JSON Report";

        downloadButton.style.cssText = `
            margin-top: 16px;
            padding: 11px 20px;
            border: none;
            border-radius: 7px;
            background: #2878d0;
            color: white;
            cursor: pointer;
            font-weight: 600;
        `;

        downloadButton.addEventListener("click", () => {

            const blob = new Blob(
                [JSON.stringify(report, null, 2)],
                { type: "application/json" }
            );

            const url = URL.createObjectURL(blob);

            const link = document.createElement("a");

            link.href = url;

            link.download =
                (latestReportId || "temperd-report") + ".json";

            link.click();

            URL.revokeObjectURL(url);

        });

        panel.appendChild(downloadButton);

        console.log("Retrieved report:", report);

    } catch (error) {

        console.error("Report retrieval error:", error);

        showMessage(
            panel,
            "Report retrieval failed: " + error.message,
            "error"
        );

    }

} */
async function displayLatestReport() {

    const reportsPage = document.getElementById("reports");

    let panel = document.getElementById("reports-result");

    if (!panel) {
        panel = document.createElement("div");
        panel.id = "reports-result";
        reportsPage.appendChild(panel);
    }

    panel.replaceChildren();

    panel.style.cssText = `
        margin-top: 24px;
        color: #e5edf8;
        overflow-wrap: anywhere;
    `;

    // ------------------------------------------
    // NO REPORT AVAILABLE
    // ------------------------------------------

    if (!latestReportUrl) {
        showMessage(
            panel,
            "No verification report available yet.",
            "info"
        );
        return;
    }

    showMessage(panel, "Loading report...", "info");

    try {

        const response = await fetch(
            `${API_BASE_URL}${latestReportUrl}`
        );

        if (!response.ok) {
            throw new Error("Unable to retrieve report.");
        }

        const report = await response.json();

        panel.replaceChildren();

        // ------------------------------------------
        // HELPER FUNCTIONS
        // ------------------------------------------

        function createElement(tag, className, text) {
            const element = document.createElement(tag);

            if (className) {
                element.className = className;
            }

            if (text !== undefined && text !== null) {
                element.textContent = String(text);
            }

            return element;
        }

        function createCard(title, value, color = "#e5edf8") {

            const card = createElement("div");
            card.style.cssText = `
                background: #111c30;
                border: 1px solid #263b5b;
                border-radius: 12px;
                padding: 20px;
                min-width: 0;
            `;

            const heading = createElement("p", "", title);
            heading.style.cssText = `
                color: #9aabc2;
                font-size: 13px;
                margin: 0 0 12px;
            `;

            const number = createElement("h2", "", value);
            number.style.cssText = `
                color: ${color};
                font-size: 28px;
                margin: 0;
                overflow-wrap: anywhere;
            `;

            card.append(heading, number);

            return card;
        }

        function createSection(title, description = "") {

            const section = createElement("section");

            section.style.cssText = `
                background: #111c30;
                border: 1px solid #263b5b;
                border-radius: 12px;
                padding: 22px;
                margin-top: 20px;
            `;

            const heading = createElement("h3", "", title);
            heading.style.cssText = `
                font-size: 17px;
                margin: 0 0 8px;
                color: #f1f5fb;
            `;

            section.appendChild(heading);

            if (description) {
                const paragraph = createElement("p", "", description);
                paragraph.style.cssText = `
                    color: #91a3bb;
                    font-size: 13px;
                    margin: 0 0 18px;
                `;
                section.appendChild(paragraph);
            }

            panel.appendChild(section);

            return section;
        }

        function createBadge(text, color) {

            const badge = createElement("span", "", text);

            badge.style.cssText = `
                display: inline-block;
                padding: 6px 12px;
                border-radius: 20px;
                background: ${color}20;
                color: ${color};
                font-size: 11px;
                font-weight: 700;
                letter-spacing: 0.5px;
            `;

            return badge;
        }

        function createInfoRow(parent, label, value) {

            const row = createElement("div");

            row.style.cssText = `
                display: flex;
                flex-wrap: wrap;
                justify-content: space-between;
                gap: 8px;
                padding: 12px 0;
                border-bottom: 1px solid #263b5b;
            `;

            const labelElement = createElement("span", "", label);
            labelElement.style.color = "#91a3bb";

            const valueElement = createElement(
                "span",
                "",
                value === undefined || value === null || value === ""
                    ? "N/A"
                    : value
            );

            valueElement.style.cssText = `
                color: #e5edf8;
                font-weight: 600;
                text-align: right;
                overflow-wrap: anywhere;
                max-width: 65%;
            `;

            row.append(labelElement, valueElement);
            parent.appendChild(row);
        }

        function createHash(parent, label, hash) {

            const container = createElement("div");

            container.style.cssText = `
                margin-top: 14px;
                padding: 14px;
                background: #0b1424;
                border-radius: 8px;
            `;

            const heading = createElement("p", "", label);
            heading.style.cssText = `
                color: #91a3bb;
                font-size: 12px;
                margin: 0 0 8px;
            `;

            const value = createElement("code", "", hash || "N/A");

            value.style.cssText = `
                display: block;
                color: #80b9ff;
                font-size: 12px;
                line-height: 1.7;
                word-break: break-all;
            `;

            container.append(heading, value);
            parent.appendChild(container);
        }

        function formatBytes(bytes) {

            if (bytes === undefined || bytes === null) {
                return "N/A";
            }

            if (bytes < 1024) {
                return bytes + " Bytes";
            }

            if (bytes < 1024 * 1024) {
                return (bytes / 1024).toFixed(2) + " KB";
            }

            return (bytes / (1024 * 1024)).toFixed(2) + " MB";
        }

        // ------------------------------------------
        // EXTRACT REPORT DATA
        // ------------------------------------------

        const files = report.files || {};
        const original = files.original || {};
        const current = files.current || {};

        const integrity = report.integrity || {};
        const blockChanges = report.block_changes || {};
        const blockDetails = blockChanges.details || [];

        const textComparison = report.text_comparison || {};
        const textChanges = textComparison.changes || [];

        const isTampered =
            String(integrity.status || "").toUpperCase() === "TAMPERED";

        const statusColor = isTampered ? "#ef6464" : "#22c58b";

        // ------------------------------------------
        // REPORT HEADER
        // ------------------------------------------

        const header = createElement("div");

        header.style.cssText = `
            background: linear-gradient(135deg, #172b49, #101b2e);
            border: 1px solid #304969;
            border-radius: 14px;
            padding: 26px;
        `;

        const topRow = createElement("div");

        topRow.style.cssText = `
            display: flex;
            flex-wrap: wrap;
            justify-content: space-between;
            align-items: center;
            gap: 14px;
        `;

        const brand = createElement("strong", "", "TEMPERD");
        brand.style.cssText = `
            font-size: 18px;
            letter-spacing: 2px;
            color: #80b9ff;
        `;

        topRow.append(
            brand,
            createBadge(
                integrity.status || "UNKNOWN",
                statusColor
            )
        );

        const title = createElement(
            "h2",
            "",
            "File Integrity Audit Report"
        );

        title.style.cssText = `
            font-size: 25px;
            margin: 24px 0 10px;
            color: #ffffff;
        `;

        const reportId = createElement(
            "p",
            "",
            "Report ID: " + (report.report_id || latestReportId || "N/A")
        );

        reportId.style.cssText = `
            color: #a8bad0;
            font-size: 13px;
            margin: 0;
        `;

        const generatedAt = createElement(
            "p",
            "",
            "Generated: " + (
                report.generated_at
                    ? new Date(report.generated_at).toLocaleString()
                    : "N/A"
            )
        );

        generatedAt.style.cssText = `
            color: #91a3bb;
            font-size: 12px;
            margin-top: 8px;
        `;

        header.append(topRow, title, reportId, generatedAt);
        panel.appendChild(header);

        // ------------------------------------------
        // EXECUTIVE SUMMARY
        // ------------------------------------------

        const summary = createSection(
            "Executive Summary",
            "Overview of the file integrity verification."
        );

        const summaryGrid = createElement("div");

        summaryGrid.style.cssText = `
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(140px, 1fr));
            gap: 12px;
        `;

        summaryGrid.append(
            createCard(
                "Modified Blocks",
                blockChanges.modified_count ?? 0,
                "#ef6464"
            ),
            createCard(
                "Added Blocks",
                blockChanges.added_count ?? 0,
                "#eab54d"
            ),
            createCard(
                "Removed Blocks",
                blockChanges.removed_count ?? 0,
                "#eab54d"
            ),
            createCard(
                "Lines Compared",
                textComparison.original_line_count ?? 0,
                "#80b9ff"
            )
        );

        summary.appendChild(summaryGrid);

        // ------------------------------------------
        // FILE INFORMATION
        // ------------------------------------------

        const fileSection = createSection(
            "File Information",
            "Comparison between the registered original and uploaded file."
        );

        const fileGrid = createElement("div");

        fileGrid.style.cssText = `
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(220px, 1fr));
            gap: 20px;
        `;

        const originalBox = createElement("div");
        const currentBox = createElement("div");

        originalBox.appendChild(
            createElement("h4", "", "Original File")
        );

        currentBox.appendChild(
            createElement("h4", "", "Current File")
        );

        createInfoRow(originalBox, "File Name", original.name);
        createInfoRow(
            originalBox,
            "File Size",
            formatBytes(original.size_bytes)
        );

        createInfoRow(currentBox, "File Name", current.name);
        createInfoRow(
            currentBox,
            "File Size",
            formatBytes(current.size_bytes)
        );

        fileGrid.append(originalBox, currentBox);
        fileSection.appendChild(fileGrid);

        // ------------------------------------------
        // MERKLE ROOT VERIFICATION
        // ------------------------------------------

        const hashSection = createSection(
            "Merkle Root Verification",
            "Cryptographic comparison of the original and current file."
        );

        hashSection.appendChild(
            createBadge(
                isTampered ? "ROOT HASH MISMATCH" : "ROOT HASH MATCH",
                statusColor
            )
        );

        createHash(
            hashSection,
            "Original Merkle Root",
            integrity.original_merkle_root
        );

        createHash(
            hashSection,
            "Current Merkle Root",
            integrity.current_merkle_root
        );

        createInfoRow(
            hashSection,
            "Block Size",
            formatBytes(integrity.block_size)
        );

        createInfoRow(
            hashSection,
            "Original Block Count",
            integrity.original_block_count
        );

        createInfoRow(
            hashSection,
            "Current Block Count",
            integrity.current_block_count
        );

        // ------------------------------------------
        // BLOCK LEVEL TAMPER LOCALIZATION
        // ------------------------------------------

        const blockSection = createSection(
            "Tamper Localization",
            "Exact file blocks where changes were detected."
        );

        if (blockDetails.length === 0) {

            const message = createElement(
                "p",
                "",
                "No block-level changes detected."
            );

            message.style.color = "#22c58b";
            blockSection.appendChild(message);

        } else {

            const tableWrapper = createElement("div");

            tableWrapper.style.cssText = `
                width: 100%;
                overflow-x: auto;
            `;

            const table = createElement("table");

            table.style.cssText = `
                width: 100%;
                border-collapse: collapse;
                min-width: 550px;
                font-size: 13px;
            `;

            const headers = [
                "Block Index",
                "Change Type",
                "Start Byte",
                "Original Size",
                "Current Size"
            ];

            const thead = createElement("thead");
            const headerRow = createElement("tr");

            headers.forEach(text => {

                const th = createElement("th", "", text);

                th.style.cssText = `
                    text-align: left;
                    padding: 13px;
                    color: #91a3bb;
                    background: #0b1424;
                    border-bottom: 1px solid #304969;
                `;

                headerRow.appendChild(th);

            });

            thead.appendChild(headerRow);

            const tbody = createElement("tbody");

            blockDetails.forEach(block => {

                const row = createElement("tr");

                const values = [
                    block.block_index,
                    block.change_type,
                    block.start_byte,
                    formatBytes(block.original_size),
                    formatBytes(block.current_size)
                ];

                values.forEach((value, index) => {

                    const td = createElement("td", "", value);

                    td.style.cssText = `
                        padding: 13px;
                        border-bottom: 1px solid #263b5b;
                        color: ${index === 1 ? "#ef8585" : "#e5edf8"};
                    `;

                    row.appendChild(td);

                });

                tbody.appendChild(row);

            });

            table.append(thead, tbody);
            tableWrapper.appendChild(table);
            blockSection.appendChild(tableWrapper);

        }

        // ------------------------------------------
        // TEXT LEVEL COMPARISON
        // ------------------------------------------

        const textSection = createSection(
            "Text-Level Comparison",
            "Line-by-line and word-level changes identified in the file."
        );

        if (!textComparison.available) {

            const message = createElement(
                "p",
                "",
                "Text comparison is not available for this file."
            );

            message.style.color = "#eab54d";
            textSection.appendChild(message);

        } else if (textChanges.length === 0) {

            const message = createElement(
                "p",
                "",
                "No text-level changes detected."
            );

            message.style.color = "#22c58b";
            textSection.appendChild(message);

        } else {

            textChanges.forEach(change => {

                const card = createElement("div");

                card.style.cssText = `
                    margin-top: 14px;
                    padding: 17px;
                    background: #0b1424;
                    border: 1px solid #263b5b;
                    border-radius: 9px;
                `;

                const heading = createElement(
                    "div",
                    "",
                    "LINE " + (change.original_line ?? "N/A") +
                    "  ·  " + (change.change_type || "MODIFIED")
                );

                heading.style.cssText = `
                    color: #80b9ff;
                    font-weight: 700;
                    font-size: 12px;
                    margin-bottom: 14px;
                `;

                const originalLabel = createElement(
                    "p",
                    "",
                    "ORIGINAL CONTENT"
                );

                originalLabel.style.cssText = `
                    color: #91a3bb;
                    font-size: 11px;
                    font-weight: 700;
                `;

                const originalText = createElement(
                    "div",
                    "",
                    change.original_text || "(Empty)"
                );

                originalText.style.cssText = `
                    background: #ef646420;
                    color: #ff9292;
                    padding: 12px;
                    border-radius: 6px;
                    margin-bottom: 12px;
                    white-space: pre-wrap;
                `;

                const currentLabel = createElement(
                    "p",
                    "",
                    "CURRENT CONTENT"
                );

                currentLabel.style.cssText = `
                    color: #91a3bb;
                    font-size: 11px;
                    font-weight: 700;
                `;

                const currentText = createElement(
                    "div",
                    "",
                    change.current_text || "(Empty)"
                );

                currentText.style.cssText = `
                    background: #22c58b20;
                    color: #62e2ad;
                    padding: 12px;
                    border-radius: 6px;
                    white-space: pre-wrap;
                `;

                card.append(
                    heading,
                    originalLabel,
                    originalText,
                    currentLabel,
                    currentText
                );

                // Removed and added words

                if (change.removed_words?.length) {

                    const removed = createElement(
                        "p",
                        "",
                        "Removed words: " +
                        change.removed_words.join(", ")
                    );

                    removed.style.cssText = `
                        color: #ff9292;
                        font-size: 12px;
                        margin-top: 12px;
                    `;

                    card.appendChild(removed);

                }

                if (change.added_words?.length) {

                    const added = createElement(
                        "p",
                        "",
                        "Added words: " +
                        change.added_words.join(", ")
                    );

                    added.style.cssText = `
                        color: #62e2ad;
                        font-size: 12px;
                        margin-top: 8px;
                    `;

                    card.appendChild(added);

                }

                textSection.appendChild(card);

            });

        }

        // ------------------------------------------
        // REPORT FOOTER
        // ------------------------------------------

        const footer = createElement(
            "p",
            "",
            "TemperD · Adaptive Merkle Tree-Based File Integrity Verification System"
        );

        footer.style.cssText = `
            text-align: center;
            color: #71849d;
            font-size: 11px;
            padding: 24px 10px;
        `;

        panel.appendChild(footer);

        // ------------------------------------------
        // DOWNLOAD JSON BUTTON
        // ------------------------------------------

        const actions = createElement("div");

        actions.style.cssText = `
            display: flex;
            flex-wrap: wrap;
            gap: 12px;
            margin-top: 20px;
        `;

        const downloadButton = createElement(
            "button",
            "",
            "↓  Download JSON"
        );

        downloadButton.type = "button";

        downloadButton.style.cssText = `
            padding: 12px 20px;
            border: none;
            border-radius: 7px;
            background: #2878d0;
            color: white;
            font-weight: 600;
            cursor: pointer;
        `;

        downloadButton.addEventListener("click", () => {

            const blob = new Blob(
                [JSON.stringify(report, null, 2)],
                { type: "application/json" }
            );

            const url = URL.createObjectURL(blob);

            const link = document.createElement("a");

            link.href = url;
            link.download =
                (report.report_id || "temperd-report") + ".json";

            link.click();

            URL.revokeObjectURL(url);

        });

        // ------------------------------------------
        // PRINT / SAVE AS PDF BUTTON
        // ------------------------------------------

        const printButton = createElement(
            "button",
            "",
            "⎙  Print / Save as PDF"
        );

        printButton.type = "button";

        printButton.style.cssText = `
            padding: 12px 20px;
            border: 1px solid #456080;
            border-radius: 7px;
            background: #1b2b43;
            color: white;
            font-weight: 600;
            cursor: pointer;
        `;

        printButton.addEventListener("click", () => {

            const printWindow = window.open("", "_blank");

            if (!printWindow) {
                alert("Please allow pop-ups to print the report.");
                return;
            }

            printWindow.document.write(`
                <!DOCTYPE html>
                <html>
                <head>
                    <title>TemperD Integrity Report</title>

                    <style>
                        body {
                            font-family: Arial, sans-serif;
                            color: #172033;
                            padding: 30px;
                            line-height: 1.5;
                        }

                        h2, h3, h4 {
                            color: #172033;
                        }

                        section {
                            border: 1px solid #d7dfeb !important;
                            background: white !important;
                            padding: 18px !important;
                            margin-top: 18px !important;
                            border-radius: 8px;
                            break-inside: avoid;
                        }

                        section p,
                        section span,
                        section div {
                            overflow-wrap: anywhere;
                        }

                        code {
                            color: #245c9c !important;
                        }

                        table {
                            width: 100%;
                            border-collapse: collapse;
                        }

                        th, td {
                            border: 1px solid #d7dfeb;
                            padding: 9px;
                            text-align: left;
                        }

                        th {
                            background: #edf2f8 !important;
                        }

                        button {
                            display: none !important;
                        }

                        @media print {
                            body {
                                padding: 10px;
                            }

                            section, header {
                                break-inside: avoid;
                            }
                        }
                    </style>
                </head>

                <body>
                    ${header.outerHTML}
                    ${Array.from(panel.children)
                        .filter(element =>
                            element !== header &&
                            element !== actions
                        )
                        .map(element => element.outerHTML)
                        .join("")}
                </body>
                </html>
            `);

            printWindow.document.close();

            printWindow.onload = () => {
                printWindow.focus();
                printWindow.print();
            };

        });

        actions.append(downloadButton, printButton);
        panel.appendChild(actions);

        console.log("Formatted TemperD report:", report);

    } catch (error) {

        console.error("Report retrieval error:", error);

        showMessage(
            panel,
            "Report retrieval failed: " + error.message,
            "error"
        );

    }

}



// ==========================================
// 13. INITIALIZATION
// ==========================================

showPage("dashboard");

console.log("TemperD frontend initialized successfully!");
