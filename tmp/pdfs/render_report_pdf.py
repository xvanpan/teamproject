from pathlib import Path
import re
from reportlab.lib import colors
from reportlab.lib.enums import TA_CENTER, TA_JUSTIFY, TA_LEFT
from reportlab.lib.pagesizes import A4
from reportlab.lib.styles import ParagraphStyle, getSampleStyleSheet
from reportlab.lib.units import mm
from reportlab.pdfbase import pdfmetrics
from reportlab.pdfbase.ttfonts import TTFont
from reportlab.platypus import (
    SimpleDocTemplate,
    Paragraph,
    Spacer,
    Table,
    TableStyle,
    PageBreak,
    Preformatted,
)


ROOT = Path(r"E:\c++ file\teamproject")
INPUT_MD = ROOT / "汇报文档_按模板版.md"
OUTPUT_DIR = ROOT / "output" / "pdf"
OUTPUT_PDF = OUTPUT_DIR / "汇报文档_按模板版.pdf"


def register_fonts():
    pdfmetrics.registerFont(TTFont("SimSun", r"C:\Windows\Fonts\simsun.ttc"))
    pdfmetrics.registerFont(TTFont("MicrosoftYaHei", r"C:\Windows\Fonts\msyh.ttc"))


def build_styles():
    styles = getSampleStyleSheet()
    return {
        "title": ParagraphStyle(
            "TitleCN",
            parent=styles["Title"],
            fontName="MicrosoftYaHei",
            fontSize=20,
            leading=28,
            alignment=TA_CENTER,
            spaceAfter=10,
        ),
        "subtitle": ParagraphStyle(
            "SubtitleCN",
            parent=styles["Heading2"],
            fontName="MicrosoftYaHei",
            fontSize=14,
            leading=20,
            alignment=TA_CENTER,
            spaceAfter=18,
        ),
        "h1": ParagraphStyle(
            "H1CN",
            parent=styles["Heading1"],
            fontName="MicrosoftYaHei",
            fontSize=15,
            leading=22,
            textColor=colors.black,
            spaceBefore=12,
            spaceAfter=8,
        ),
        "h2": ParagraphStyle(
            "H2CN",
            parent=styles["Heading2"],
            fontName="MicrosoftYaHei",
            fontSize=13,
            leading=20,
            textColor=colors.black,
            spaceBefore=10,
            spaceAfter=6,
        ),
        "h3": ParagraphStyle(
            "H3CN",
            parent=styles["Heading3"],
            fontName="MicrosoftYaHei",
            fontSize=11.5,
            leading=18,
            textColor=colors.black,
            spaceBefore=8,
            spaceAfter=5,
        ),
        "body": ParagraphStyle(
            "BodyCN",
            parent=styles["BodyText"],
            fontName="SimSun",
            fontSize=10.5,
            leading=18,
            alignment=TA_JUSTIFY,
            firstLineIndent=21,
            spaceAfter=4,
        ),
        "body_no_indent": ParagraphStyle(
            "BodyNoIndentCN",
            parent=styles["BodyText"],
            fontName="SimSun",
            fontSize=10.5,
            leading=18,
            alignment=TA_LEFT,
            firstLineIndent=0,
            spaceAfter=4,
        ),
        "code": ParagraphStyle(
            "CodeCN",
            parent=styles["Code"],
            fontName="SimSun",
            fontSize=8.8,
            leading=12,
            backColor=colors.HexColor("#F5F5F5"),
            borderPadding=6,
            leftIndent=6,
            rightIndent=6,
            spaceBefore=4,
            spaceAfter=6,
        ),
    }


def escape_html(text: str) -> str:
    return (
        text.replace("&", "&amp;")
        .replace("<", "&lt;")
        .replace(">", "&gt;")
    )


def parse_markdown_table(lines, start):
    rows = []
    i = start
    while i < len(lines) and lines[i].strip().startswith("|"):
        raw = lines[i].strip().strip("|")
        cells = [cell.strip() for cell in raw.split("|")]
        rows.append(cells)
        i += 1
    if len(rows) >= 2 and all(re.fullmatch(r"[:\- ]+", c) for c in rows[1]):
        rows.pop(1)
    return rows, i


def make_table(rows):
    table = Table(rows, repeatRows=1)
    table.setStyle(
        TableStyle(
            [
                ("FONTNAME", (0, 0), (-1, -1), "SimSun"),
                ("FONTSIZE", (0, 0), (-1, -1), 9.5),
                ("LEADING", (0, 0), (-1, -1), 14),
                ("BACKGROUND", (0, 0), (-1, 0), colors.HexColor("#EAEAEA")),
                ("TEXTCOLOR", (0, 0), (-1, -1), colors.black),
                ("GRID", (0, 0), (-1, -1), 0.5, colors.grey),
                ("VALIGN", (0, 0), (-1, -1), "TOP"),
                ("LEFTPADDING", (0, 0), (-1, -1), 6),
                ("RIGHTPADDING", (0, 0), (-1, -1), 6),
                ("TOPPADDING", (0, 0), (-1, -1), 5),
                ("BOTTOMPADDING", (0, 0), (-1, -1), 5),
            ]
        )
    )
    return table


def build_story(md_text, styles):
    lines = md_text.splitlines()
    story = []
    i = 0
    in_code = False
    code_lines = []

    while i < len(lines):
        line = lines[i]
        stripped = line.strip()

        if stripped.startswith("```"):
            if not in_code:
                in_code = True
                code_lines = []
            else:
                story.append(Preformatted("\n".join(code_lines), styles["code"]))
                story.append(Spacer(1, 4))
                in_code = False
            i += 1
            continue

        if in_code:
            code_lines.append(line)
            i += 1
            continue

        if not stripped:
            story.append(Spacer(1, 6))
            i += 1
            continue

        if stripped.startswith("|"):
            rows, i = parse_markdown_table(lines, i)
            if rows:
                story.append(make_table(rows))
                story.append(Spacer(1, 8))
            continue

        if stripped.startswith("# "):
            story.append(Paragraph(escape_html(stripped[2:].strip()), styles["title"]))
            i += 1
            continue

        if stripped.startswith("## "):
            story.append(Paragraph(escape_html(stripped[3:].strip()), styles["subtitle"]))
            i += 1
            continue

        if stripped.startswith("### "):
            story.append(Paragraph(escape_html(stripped[4:].strip()), styles["h1"]))
            i += 1
            continue

        if stripped.startswith("#### "):
            story.append(Paragraph(escape_html(stripped[5:].strip()), styles["h2"]))
            i += 1
            continue

        if stripped.startswith("##### ") or stripped.startswith("###### "):
            text = stripped.split(" ", 1)[1]
            story.append(Paragraph(escape_html(text), styles["h3"]))
            i += 1
            continue

        text = escape_html(stripped)
        text = re.sub(r"`([^`]+)`", r"<font face='MicrosoftYaHei'>\1</font>", text)
        text = text.replace("  ", "<br/>")

        bullet_match = re.match(r"^(\d+)\.\s+(.*)$", stripped)
        dash_match = re.match(r"^-\s+(.*)$", stripped)
        if bullet_match:
            text = f"{bullet_match.group(1)}. {escape_html(bullet_match.group(2))}"
            story.append(Paragraph(text, styles["body_no_indent"]))
        elif dash_match:
            text = f"- {escape_html(dash_match.group(1))}"
            story.append(Paragraph(text, styles["body_no_indent"]))
        else:
            story.append(Paragraph(text, styles["body"]))
        i += 1

    return story


def add_page_number(canvas, doc):
    canvas.saveState()
    canvas.setFont("SimSun", 9)
    canvas.drawCentredString(A4[0] / 2.0, 12 * mm, f"{doc.page}")
    canvas.restoreState()


def main():
    register_fonts()
    styles = build_styles()
    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)
    md_text = INPUT_MD.read_text(encoding="utf-8")

    doc = SimpleDocTemplate(
        str(OUTPUT_PDF),
        pagesize=A4,
        leftMargin=22 * mm,
        rightMargin=22 * mm,
        topMargin=22 * mm,
        bottomMargin=20 * mm,
        title="汇报文档_按模板版",
        author="Codex",
    )

    story = build_story(md_text, styles)
    doc.build(story, onFirstPage=add_page_number, onLaterPages=add_page_number)
    print(str(OUTPUT_PDF))


if __name__ == "__main__":
    main()
